// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ItemSubsystem.h"
#include "ItemSystemLibrary.h"
#include "ItemTypes.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    InventoryCapacity = 30;
}

void UInventoryManagerComponent::BeginPlay()
{
    Super::BeginPlay();

    ItemSubsystem = UItemSystemLibrary::GetItemSubsystem();
    if (!ItemSubsystem)
    {
        UE_LOG(LogTemp, Error, TEXT("InventoryManagerComponent: Failed to get MyItemSubsystem!"));
    }

    AActor* Owner = GetOwner();
    if (Owner)
    {
        OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
    }

    InitializeInventorySlots();
}

void UInventoryManagerComponent::InitializeInventorySlots()
{
    InventorySlots.SetNum(InventoryCapacity);
}

int32 UInventoryManagerComponent::AddItem(FName ItemIDToAdd, int32 QuantityToAdd)
{
    if (!ItemSubsystem || ItemIDToAdd.IsNone() || QuantityToAdd <= 0)
    {
        return 0;
    }
    
    int32 QuantityAdded = 0;
    int32 QuantityLeftToAdd = QuantityToAdd;
    int32 FoundMaxStackSize = -1;
    TOptional<FDataTableRowHandle> FoundConsumableHandle;

    for (int32 SlotIndex = 0; SlotIndex < InventorySlots.Num(); ++SlotIndex)
    {
        FInventorySlotData& Slot = InventorySlots[SlotIndex];

        if (Slot.ItemID == ItemIDToAdd)
        {
            if (FoundMaxStackSize < 0)
            {
                FoundMaxStackSize = Slot.MaxStackSize;
                if (FoundMaxStackSize <= 0)
                {
                    FoundMaxStackSize = 1;
                }

                if (Slot.ConsumableComponentHandle.IsSet())
                {
                    FoundConsumableHandle = Slot.ConsumableComponentHandle;
                }
            }

            if (FoundMaxStackSize > 1 && Slot.Quantity < FoundMaxStackSize)
            {
                int32 CanAddToSlot = FoundMaxStackSize - Slot.Quantity;
                int32 AddAmount = FMath::Min(QuantityLeftToAdd, CanAddToSlot);

                Slot.Quantity += AddAmount;
                QuantityAdded += AddAmount;
                QuantityLeftToAdd -= AddAmount;

                if (QuantityLeftToAdd <= 0)
                {
                    // TODO: 인벤토리 변경 델리게이트 호출
                    // OnInventoryChanged.Broadcast();
                    return QuantityAdded;
                }
            }
        }
    }

    if (QuantityLeftToAdd > 0)
    {
        if (FoundMaxStackSize < 0)
        {
            const FItemBaseInfoRow* BaseInfo = ItemSubsystem->GetItemBaseData(ItemIDToAdd);
            if (!BaseInfo)
            {
                UE_LOG(LogTemp, Warning, TEXT("AddItem: Base item data not found for %s. Cannot add to empty slot."), *ItemIDToAdd.ToString());
                // OnInventoryChanged.Broadcast();
                return QuantityAdded; // 이전 단계에서 추가된 것만 반환
            }

            // 인벤토리 정보 조회 (MaxStackSize 용도)
            const FInventoryComponentRow* InvData = UItemSystemLibrary::FindComponentDataRowInHandles<FInventoryComponentRow>(BaseInfo->ItemComponentHandles, TEXT("AddItem_FetchInvData"));
            if (!InvData)
            {
                UE_LOG(LogTemp, Warning, TEXT("AddItem: Inventory component data not found for %s. Assuming MaxStackSize=1."), *ItemIDToAdd.ToString());
                FoundMaxStackSize = 1;
            }
            else
            {
                FoundMaxStackSize = InvData->MaxStackSize;
                if (FoundMaxStackSize <= 0) FoundMaxStackSize = 1;
            }

            // 소비 정보 핸들 조회
            for (const FDataTableRowHandle& Handle : BaseInfo->ItemComponentHandles)
            {
                if (!Handle.IsNull() && Handle.DataTable && Handle.DataTable->GetRowStruct() == FConsumableComponentRow::StaticStruct())
                {
                    FoundConsumableHandle = Handle;
                    break; // 소비 핸들은 하나만 있다고 가정 (필요시 여러 개 처리)
                }
            }
            // 여기서 다른 필요한 핸들들(CraftingMaterialComponentHandle 등)도 유사하게 찾아 Found... 변수에 저장할 수 있음
        }

        // 이제 FoundMaxStackSize와 FoundConsumableHandle은 값이 있거나, 없으면 기본값/unset 상태임
        while (QuantityLeftToAdd > 0)
        {
            int32 EmptySlotIndex = FindEmptySlot();
            if (EmptySlotIndex != INDEX_NONE)
            {
                FInventorySlotData& NewSlot = InventorySlots[EmptySlotIndex];
                int32 AddAmount = FMath::Min(QuantityLeftToAdd, FoundMaxStackSize);

                NewSlot.ItemID = ItemIDToAdd;
                NewSlot.Quantity = AddAmount;
                NewSlot.MaxStackSize = FoundMaxStackSize; // MaxStackSize 캐싱
                NewSlot.ConsumableComponentHandle = FoundConsumableHandle; // 소비 핸들 캐싱 (있으면 설정, 없으면 Reset된 상태)
                // NewSlot.CraftingMaterialComponentHandle = FoundCraftingHandle; // 다른 핸들도 캐싱

                QuantityAdded += AddAmount;
                QuantityLeftToAdd -= AddAmount;
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("AddItem: Inventory is full. Could not add remaining %d of %s"), QuantityLeftToAdd, *ItemIDToAdd.ToString());
                break;
            }
        }
    }

    // TODO: 인벤토리 변경 델리게이트 호출
    // OnInventoryChanged.Broadcast();

    return QuantityAdded;
}

int32 UInventoryManagerComponent::RemoveItemFromSlot(int32 SlotIndex, int32 QuantityToRemove /*= 0*/)
{
    if (!InventorySlots.IsValidIndex(SlotIndex) || InventorySlots[SlotIndex].IsEmpty())
    {
        return 0;
    }

    int32 CurrentQuantity = InventorySlots[SlotIndex].Quantity;
    int32 RemoveAmount = (QuantityToRemove <= 0 || QuantityToRemove >= CurrentQuantity) ? CurrentQuantity : QuantityToRemove;

    InventorySlots[SlotIndex].Quantity -= RemoveAmount;

    if (InventorySlots[SlotIndex].Quantity <= 0)
    {
        InventorySlots[SlotIndex].Clear(); // 슬롯 비우기
    }

    // TODO: 인벤토리 변경 델리게이트 호출
    // OnInventoryChanged.Broadcast();

    return RemoveAmount;
}

void UInventoryManagerComponent::UseItemInSlot(int32 SlotIndex)
{
    FInventorySlotData SlotData;
    if (!GetSlotDataByIndex(SlotIndex, SlotData) || SlotData.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("UseItemInSlot: Invalid or empty slot index %d"), SlotIndex);
        return;
    }
    
    if (SlotData.ConsumableComponentHandle.IsSet())
    {
        FDataTableRowHandle ConsumeHandle = SlotData.ConsumableComponentHandle.GetValue();

        const FConsumableComponentRow* ConsumeData = ConsumeHandle.GetRow<FConsumableComponentRow>(TEXT("UseItemInSlot_GetConsumeData"));

        if (ConsumeData)
        {
            HandleItemConsumption(ConsumeData);

            // 아이템 소모 처리 (예: 수량 감소)
            if (ConsumeData->MaxCharges <= 1) // 1회용 아이템이라고 가정
            {
                RemoveItemFromSlot(SlotIndex, 1);
            }
            else
            {
                // TODO: 충전 횟수 기반 아이템 처리 (FInventorySlotData에 현재 충전량 저장 필요)
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UseItemInSlot: ConsumableComponentHandle for item %s in slot %d pointed to an invalid row or DataTable."), *SlotData.ItemID.ToString(), SlotIndex);
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("UseItemInSlot: Item %s in slot %d is not consumable (no ConsumableComponentHandle)."), *SlotData.ItemID.ToString(), SlotIndex);
        // 다른 유형의 아이템 사용 로직 (예: 장착 요청)
    }
}


bool UInventoryManagerComponent::GetSlotDataByIndex(int32 SlotIndex, FInventorySlotData& OutSlotData) const
{
    if (InventorySlots.IsValidIndex(SlotIndex))
    {
        OutSlotData = InventorySlots[SlotIndex];
        return true;
    }
    return false;
}

int32 UInventoryManagerComponent::FindEmptySlot() const
{
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        if (InventorySlots[i].IsEmpty())
        {
            return i;
        }
    }
    return INDEX_NONE;
}

bool UInventoryManagerComponent::IsSlotFull(int32 SlotIndex) const
{
    if (InventorySlots.IsValidIndex(SlotIndex))
    {
        return InventorySlots[SlotIndex].Quantity >= InventorySlots[SlotIndex].MaxStackSize;
    }
    return true;
}

void UInventoryManagerComponent::HandleItemConsumption(const FConsumableComponentRow* ConsumeData)
{
    if (!ConsumeData || !OwnerAbilitySystemComponent) return;

    AActor* Owner = GetOwner(); // 효과의 출처가 될 수 있음
    UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, ConsumeData->EffectsToApplyOnConsume, Owner); 
}
