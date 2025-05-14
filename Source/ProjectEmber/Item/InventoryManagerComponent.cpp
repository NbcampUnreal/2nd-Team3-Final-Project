// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagerComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ItemSubsystem.h"
#include "Core/EmberItemStruct.h"
#include "Core/ItemSystemLibrary.h"
#include "EmberLog/EmberLog.h"

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
        EMBER_LOG(LogTemp, Error, TEXT("InventoryManagerComponent: Failed to get MyItemSubsystem!"));
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

int32 UInventoryManagerComponent::AddItemAndHandleOverflow(FName ItemIDToAdd, int32 QuantityToAdd, FVector DropLocation, FRotator DropRotation)
{
    if (ItemIDToAdd.IsNone() || QuantityToAdd <= 0)
    {
        return 0;
    }

    const int32 QuantityActuallyAddedToSlots = TryAddItemsToSlots(ItemIDToAdd, QuantityToAdd);

    const int32 QuantityLeftToDrop = QuantityToAdd - QuantityActuallyAddedToSlots;

    if (QuantityLeftToDrop > 0)
    {
        EMBER_LOG(LogTemp, Log, TEXT("Inventory full or partial add. Dropping %d of %s"), QuantityLeftToDrop, *ItemIDToAdd.ToString());
        SpawnDroppedItem(ItemIDToAdd, QuantityLeftToDrop, DropLocation, DropRotation);
    }
    

    return QuantityActuallyAddedToSlots;
}

int32 UInventoryManagerComponent::TryAddItemsToSlots(FName ItemIDToAdd, int32 QuantityToAdd)
{
    if (!ItemSubsystem || ItemIDToAdd.IsNone() || QuantityToAdd <= 0)
    {
        return 0;
    }
    
    int32 QuantityAdded = 0;
    int32 QuantityLeftToAdd = QuantityToAdd;

    
    TOptional<FInventorySlotData> TmpInventorySlotData;
    
    for (int32 SlotIndex = 0; SlotIndex < InventorySlots.Num(); ++SlotIndex)
    {
        FInventorySlotData& Slot = InventorySlots[SlotIndex];

        if (Slot.ItemID == ItemIDToAdd)
        {
            if (!TmpInventorySlotData.IsSet())
            {
                TmpInventorySlotData = Slot;
            }
            
            int32 CurrentMaxStack = Slot.MaxStackSize; // 또는 TemplateSlotDataForExistingItem.GetValue().CachedMaxStackSize
            if (CurrentMaxStack > 1 && Slot.Quantity < CurrentMaxStack)
            {
                int32 CanAddToSlot = CurrentMaxStack - Slot.Quantity;
                int32 AddAmount = FMath::Min(QuantityLeftToAdd, CanAddToSlot);

                Slot.Quantity += AddAmount;
                QuantityAdded += AddAmount;
                QuantityLeftToAdd -= AddAmount;
                
                OnInventoryChanged.Broadcast(SlotIndex, Slot);
                
                if (QuantityLeftToAdd <= 0)
                {
                    return QuantityAdded;
                }
            }
        }
    }

    if (QuantityLeftToAdd > 0)
    {
        if (!TmpInventorySlotData.IsSet())
        {
            if (const FItemMasterInfoRow* BaseInfo = ItemSubsystem->GetItemMasterInfoRow(ItemIDToAdd))
            {
                TmpInventorySlotData = FInventorySlotData(ItemIDToAdd);
            }
            else
            {
                EMBER_LOG(LogTemp, Warning, TEXT("AddItem: Base item data not found for %s. Cannot add to empty slot."), *ItemIDToAdd.ToString());
                return QuantityAdded;
            }
        }

        while (QuantityLeftToAdd > 0)
        {
            int32 EmptySlotIndex = FindEmptySlot();
            if (EmptySlotIndex != INDEX_NONE)
            {
                FInventorySlotData& NewSlot = InventorySlots[EmptySlotIndex];
                int32 AddAmount = FMath::Min(QuantityLeftToAdd, TmpInventorySlotData->MaxStackSize);

                NewSlot = FInventorySlotData(*TmpInventorySlotData);
                NewSlot.Quantity = AddAmount;
                EMBER_LOG(LogTemp, Warning, TEXT("Slot2: %s"), *TmpInventorySlotData->ItemID.ToString());

                OnInventoryChanged.Broadcast(EmptySlotIndex, NewSlot);
                
                QuantityAdded += AddAmount;
                QuantityLeftToAdd -= AddAmount;
            }
            else
            {
                EMBER_LOG(LogTemp, Warning, TEXT("AddItem: Inventory is full. Could not add remaining %d of %s"), QuantityLeftToAdd, *ItemIDToAdd.ToString());
                break;
            }
        }
    }

    return QuantityAdded;
}

void UInventoryManagerComponent::SpawnDroppedItem(FName ItemIDToDrop, int32 QuantityToDrop, FVector SpawnLocation, FRotator SpawnRotation)
{
    /*if (!DroppedItemActorClass)
    {
        EMBER_LOG(LogTemp, Warning, TEXT("SpawnDroppedItem: DroppedItemActorClass is not set in InventoryManagerComponent. Cannot spawn item %s."), *ItemIDToDrop.ToString());
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = GetOwner() ? GetOwner()->GetInstigator() : nullptr;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // 액터 스폰
    AMyDroppedItemActor* DroppedItem = World->SpawnActor<AMyDroppedItemActor>(DroppedItemActorClass, SpawnLocation, SpawnRotation, SpawnParams);

    if (DroppedItem)
    {
        // 스폰된 액터에 아이템 정보 전달 (AMyDroppedItemActor에 해당 함수/변수 필요)
        // 예: DroppedItem->InitializeDroppedItem(ItemIDToDrop, QuantityToDrop);
        EMBER_LOG(LogTemp, Log, TEXT("SpawnDroppedItem: Spawned %s (Quantity: %d) at %s"), *ItemIDToDrop.ToString(), QuantityToDrop, *SpawnLocation.ToString());
    }
    else
    {
        EMBER_LOG(LogTemp, Error, TEXT("SpawnDroppedItem: Failed to spawn DroppedItemActor for %s."), *ItemIDToDrop.ToString());
    }*/
}

int32 UInventoryManagerComponent::GetSlotCount() const
{
    return InventoryCapacity;
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
    OnInventoryChanged.Broadcast(SlotIndex, InventorySlots[SlotIndex]);

    return RemoveAmount;
}

void UInventoryManagerComponent::UseItemInSlot(int32 SlotIndex)
{
    FInventorySlotData SlotData;
    if (!GetSlotDataByIndex(SlotIndex, SlotData) || SlotData.IsEmpty())
    {
        EMBER_LOG(LogTemp, Warning, TEXT("UseItemInSlot: Invalid or empty slot index %d"), SlotIndex);
        return;
    }
    
    if (SlotData.ConsumableData.IsSet())
    {
        FDataTableRowHandle ConsumeHandle = SlotData.ConsumableData.GetValue();

        

        if (const FConsumableInfoRow* ConsumeData = ConsumeHandle.GetRow<FConsumableInfoRow>(TEXT("UseItemInSlot_GetConsumeData")))
        {
            HandleItemConsumption(ConsumeData);
            RemoveItemFromSlot(SlotIndex, ConsumeData->ConsumeAmount);
        }
        else
        {
            EMBER_LOG(LogTemp, Error, TEXT("UseItemInSlot: ConsumableComponentHandle for item %s in slot %d pointed to an invalid row or DataTable."), *SlotData.ItemID.ToString(), SlotIndex);
        }
    }
    else
    {
        EMBER_LOG(LogTemp, Log, TEXT("UseItemInSlot: Item %s in slot %d is not consumable (no ConsumableComponentHandle)."), *SlotData.ItemID.ToString(), SlotIndex);
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

void UInventoryManagerComponent::HandleItemConsumption(const FConsumableInfoRow* ConsumeData)
{
    if (!ConsumeData || !OwnerAbilitySystemComponent) return;

    AActor* Owner = GetOwner(); // 효과의 출처가 될 수 있음
    UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, ConsumeData->EffectsToApplyOnConsume, Owner); 
}
