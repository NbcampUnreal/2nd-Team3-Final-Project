// Fill out your copyright notice in the Description page of Project Settings.


#include "EmberDataContainer.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Core/EmberItemStruct.h"
#include "Core/ItemSystemLibrary.h"
#include "Containers/Queue.h"
#include "EmberLog/EmberLog.h"


// Sets default values for this component's properties
UEmberDataContainer::UEmberDataContainer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	// ...
}

void UEmberDataContainer::InitSlot(int32 InSlotMax, int32 InSlotMaxRow, TObjectPtr<AActor> InOwner)
{
 
    SlotCapacity = InSlotMax;
    SlotMaxRow = InSlotMaxRow;

    Owner = InOwner;
    if (Owner)
    {
        OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);
    }
#if UE_BUILD_DEVELOPMENT
    if (!OwnerAbilitySystemComponent)
    {
        EMBER_LOG(LogEmberItem, Warning, TEXT("OwnerAbilitySystemComponent: Failed to get AbilitySystem!"));
    }
#endif
    
    InitializeInventorySlots();
}

int32 UEmberDataContainer::MergeSameItemSlot(int32 SlotIndexTo ,int32 SlotIndexFrom)
{
    if (!DataSlots.IsValidIndex(SlotIndexTo) || !DataSlots.IsValidIndex(SlotIndexFrom))
    {
        EMBER_LOG(LogEmberItem, Error, TEXT("UEmberDataContainer::MergeItemSlot: Invalid Index"));
        return 0;
    }

    
    FEmberSlotData* SlotFrom = DataSlots[SlotIndexFrom].GetMutablePtr<FEmberSlotData>();
    FEmberSlotData* SlotTo = DataSlots[SlotIndexTo].GetMutablePtr<FEmberSlotData>();
    int MovedAmount = 0;
    if (SlotFrom && SlotTo)
    {
        if (SlotFrom->ItemID == SlotTo->ItemID)
        {
            int CurrentMaxQuantity = SlotTo->MaxStackSize - SlotTo->Quantity;
            MovedAmount = FMath::Min(SlotFrom->Quantity, CurrentMaxQuantity);
            MovedAmount = FMath::Max(MovedAmount, 0);
                
            SlotTo->Quantity += MovedAmount;
            SlotFrom->Quantity -= MovedAmount;
            if (SlotFrom->Quantity <= 0)
            {
                SlotFrom->Clear();
            }
            OnDataChangedDelegate.Broadcast(SlotIndexFrom, DataSlots[SlotIndexFrom]);
            OnDataChangedDelegate.Broadcast(SlotIndexTo, DataSlots[SlotIndexTo]);

            return MovedAmount;
        }
    }
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("MovedAmount : %d"), MovedAmount);

#endif
    return MovedAmount;
}

void UEmberDataContainer::SwapInventorySlots(int32 SlotIndex1, int32 SlotIndex2)
{
    if (!DataSlots.IsValidIndex(SlotIndex1) || !DataSlots.IsValidIndex(SlotIndex2))
    {
        EMBER_LOG(LogEmberItem, Error, TEXT("Invalid Index"));
        return;
    }

    DataSlots.Swap(SlotIndex1, SlotIndex2);
    OnDataChangedDelegate.Broadcast(SlotIndex1, DataSlots[SlotIndex1]);
    OnDataChangedDelegate.Broadcast(SlotIndex2, DataSlots[SlotIndex2]);
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("SlotIndex1 : %d, SlotIndex2 : %d"), SlotIndex1, SlotIndex2);

#endif
}

int32 UEmberDataContainer::AddDataInIndex(const FItemPair& InItem, int32 InSlotIndex)
{
    int32 CurrentQuantity = 0;
    if (DataSlots.IsValidIndex(InSlotIndex))
    {
        if (FEmberSlotData* Slot = DataSlots[InSlotIndex].GetMutablePtr<FEmberSlotData>())
        {
            if (Slot->ItemID.IsNone())
            {
                *Slot = FEmberSlotData(InItem.ItemID);
            }
            
            if (InItem.ItemID == Slot->ItemID)
            {
                CurrentQuantity = FMath::Min(Slot->MaxStackSize - Slot->Quantity, InItem.Quantity);
                CurrentQuantity = FMath::Max(CurrentQuantity, 0);

                Slot->Quantity += CurrentQuantity;

                
                Slot->EnchantEffects = InItem.Enchants;
                
                FTotalItemInfo& Total = TotalData.FindOrAdd(Slot->ItemID);
                Total.AddItem(CurrentQuantity, InSlotIndex);
                
                OnDataChangedDelegate.Broadcast(InSlotIndex, DataSlots[InSlotIndex]);
            }
            
        }
    }

#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("ReturnQuantity : %d"), CurrentQuantity);

#endif

    
    return CurrentQuantity;
}

void UEmberDataContainer::FindSlotIndexesByItemID(const FName& ItemID, TQueue<int32>& OutSlotIndexes,
    int32 InBeginIndex)
{
    if (!DataSlots.IsValidIndex(InBeginIndex))
    {
        InBeginIndex = 0;
    }
    for (int32 Index = InBeginIndex; Index < DataSlots.Num() ; ++Index)
    {
        const FInstancedStruct& SlotInstance = DataSlots[Index];
        if (const FEmberSlotData* Slot = SlotInstance.GetPtr<FEmberSlotData>())
        {
            if (Slot->ItemID == ItemID && !Slot->bIsFull())
            {
                OutSlotIndexes.Enqueue(Index);
            }
        }
    }
#if UE_BUILD_DEVELOPMENT
    if (OutSlotIndexes.IsEmpty())
    {
        EMBER_LOG(LogEmberItem, Display, TEXT("Queue Is Empty"));
    }
    else
    {
        EMBER_LOG(LogEmberItem, Display, TEXT("Queue Is Avalid"));
    }

#endif
}

void UEmberDataContainer::FindEmptySlotIndexes(TQueue<int32>& OutSlotIndexes) const
{
    for (int32 Index = 0; Index < DataSlots.Num(); Index++)
    {
        const FInstancedStruct& SlotInstance = DataSlots[Index];
        if (const FEmberSlotData* Slot = SlotInstance.GetPtr<FEmberSlotData>())
        {
            if (Slot->bIsEmpty())
            {
                OutSlotIndexes.Enqueue(Index);
            }
            else
            {
                EMBER_LOG(LogEmberItem, Display, TEXT("Slot : %s, %d"), *Slot->ItemID.ToString(), Slot->Quantity);

            }
        }
    }
    
#if UE_BUILD_DEVELOPMENT
    if (OutSlotIndexes.IsEmpty())
    {
        EMBER_LOG(LogEmberItem, Display, TEXT("Queue Is Empty"));
    }
    else
    {
        EMBER_LOG(LogEmberItem, Display, TEXT("Queue Is Avalid"));
    }

#endif
}

int32 UEmberDataContainer::AddItemSlotIndex(FEmberSlotData& Slot, int32 QuantityToAdd) const
{
    int32 AddAmount = FMath::Min(QuantityToAdd, Slot.MaxStackSize - Slot.Quantity);
    AddAmount = FMath::Max(AddAmount, 0);

    Slot.Quantity += AddAmount;
    
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("Return Amount : %d"), AddAmount);
#endif
    
    return AddAmount;
}

void UEmberDataContainer::InitializeInventorySlots()
{
    DataSlots.SetNum(SlotCapacity);
    for (FInstancedStruct& Data : DataSlots)
    {
        FEmberSlotData NewSlot;
        NewSlot.InitializeInstancedStruct(Data);
    }
       
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("SlotCount : %d"), DataSlots.Num());
#endif
    
}

int32 UEmberDataContainer::AddItem_Implementation(const FItemPair& InItem, int32 InSlotIndex)
{
    if (InItem.ItemID.IsNone() || InItem.Quantity <= 0)
    {
        return 0;
    }

    const int32 QuantityActuallyAddedToSlots = TryAddItemsToSlots(InItem, InSlotIndex);

    const int32 QuantityLeftToDrop = InItem.Quantity - QuantityActuallyAddedToSlots;

    if (QuantityLeftToDrop > 0 && Owner)
    {
        EMBER_LOG(LogEmberItem, Log, TEXT("Inventory full or partial add. Dropping %d of %s"), QuantityLeftToDrop, *InItem.ItemID.ToString());
        SpawnDroppedItem(InItem.ItemID, QuantityLeftToDrop, Owner->GetActorLocation(), FRotator::ZeroRotator);
    }

    
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("ReturnAmount : %d"), QuantityActuallyAddedToSlots);
#endif
    
    return QuantityActuallyAddedToSlots;
}

TArray<FItemPair> UEmberDataContainer::AddItems_Implementation(const TArray<FItemPair>& Items)
{
    TArray<FItemPair> RemainItems;
    for (const FItemPair& Item : Items)
    {
        int32 Remain = Item.Quantity - TryAddItemsToSlots(Item);
        if (Remain > 0)
        {
            RemainItems.Add(FItemPair(Item.ItemID, Remain));
        }
    }

    
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("ReturnRemainNum : %d"), RemainItems.Num());
#endif
    
    
    return RemainItems;
}

int32 UEmberDataContainer::TryAddItemsToSlots(const FItemPair& InItem, int32 InSlotIndex)
{
    
    if (InItem.ItemID.IsNone() || InItem.Quantity <= 0)
    {
        return 0;
    }

    int32 QuantityAdded = 0;
    FItemPair CurrentItem = InItem;
    
    if (DataSlots.IsValidIndex(InSlotIndex))
    {
        QuantityAdded = AddDataInIndex(CurrentItem, InSlotIndex);

        CurrentItem.Quantity -= QuantityAdded;
        if (QuantityAdded > 0)
        {
            OnDataChangedDelegate.Broadcast(InSlotIndex, DataSlots[InSlotIndex]);
        }
    }
    TQueue<int32> TargetSlotIndexes;
    FindSlotIndexesByItemID(InItem.ItemID, TargetSlotIndexes, InSlotIndex);
    FindEmptySlotIndexes(TargetSlotIndexes);

    
    EMBER_LOG(LogEmberItem, Display, TEXT("SlotIndex : %d"), *TargetSlotIndexes.Peek());
    if (TargetSlotIndexes.IsEmpty())
    {
        return 0;
    }
    if (int32* SlotIndex = TargetSlotIndexes.Peek())
    {
        while (SlotIndex && CurrentItem.Quantity > 0 && DataSlots.IsValidIndex(*SlotIndex))
        {
            int32 CurrentAddedQuantity = AddDataInIndex(CurrentItem, *SlotIndex);

            QuantityAdded += CurrentAddedQuantity;
            CurrentItem.Quantity -= CurrentAddedQuantity;

            if (CurrentAddedQuantity > 0)
            {
                OnDataChangedDelegate.Broadcast(*SlotIndex, DataSlots[*SlotIndex]);
            }
            if (!TargetSlotIndexes.Dequeue(*SlotIndex) || CurrentItem.Quantity <= 0)
            {
                break;
            }
        }
    }

    
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("ReturnRemainNum : %d"), QuantityAdded);
#endif
    
    return QuantityAdded;
}

void UEmberDataContainer::SpawnDroppedItem(FName ItemIDToDrop, int32 QuantityToDrop, FVector SpawnLocation, FRotator SpawnRotation)
{
    /*if (!DroppedItemActorClass)
    {
        EMBER_LOG(LogEmberItem, Warning, TEXT("SpawnDroppedItem: DroppedItemActorClass is not set in InventoryManagerComponent. Cannot spawn item %s."), *ItemIDToDrop.ToString());
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
        EMBER_LOG(LogEmberItem, Log, TEXT("SpawnDroppedItem: Spawned %s (Quantity: %d) at %s"), *ItemIDToDrop.ToString(), QuantityToDrop, *SpawnLocation.ToString());
    }
    else
    {
        EMBER_LOG(LogEmberItem, Error, TEXT("SpawnDroppedItem: Failed to spawn DroppedItemActor for %s."), *ItemIDToDrop.ToString());
    }*/
}

int32 UEmberDataContainer::GetSlotCount_Implementation() const
{
    return SlotCapacity;
}

FGameplayTag UEmberDataContainer::GetSlotType_Implementation() const
{
    return SlotTag;
}

int32 UEmberDataContainer::RemoveItemAutomatic(const FItemPair& InItem)
{
    if (InItem.Quantity <= 0 || InItem.ItemID.IsNone())
    {
        EMBER_LOG(LogEmberItem, Warning, TEXT("Non Valid InParameter"))
        return 0;
    }

    int InRemoveQuantity = InItem.Quantity;
    int RemovedQuantity = 0;   
    if (FTotalItemInfo* TotalItemInfo = TotalData.Find(InItem.ItemID))
    {
        TSet<int32> ItemIndexes = TotalItemInfo->ItemIndexes;
        for (int32 Index : ItemIndexes)
        {
            RemovedQuantity = RemoveItemFromSlot_Implementation(Index, InRemoveQuantity);
            InRemoveQuantity -= RemovedQuantity;
            if (InRemoveQuantity <= 0)
            {
                break;
            }
        }
    }
    return RemovedQuantity;
}


void UEmberDataContainer::MoveItemByIndex(int32 IndexTo, int32 IndexForm, int32 InQuantity)
{
    if (!DataSlots.IsValidIndex(IndexTo) ||
        !DataSlots.IsValidIndex(IndexForm))
    {
        EMBER_LOG(LogEmber, Warning, TEXT("CombineItemByIndex: Index is not valid. To : %d, Form : %d"), IndexTo, IndexForm);
        return;
    }

    int32 MovedQuantity = MergeSameItemSlot(IndexTo, IndexForm);

    if (MovedQuantity <= 0)
    {
        SwapInventorySlots(IndexTo, IndexForm);
    }

       
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("MovedQuantity : %d"), MovedQuantity);
#endif
   
}

int32 UEmberDataContainer::RemoveItemFromSlot_Implementation(int32 SlotIndex, int32 QuantityToRemove)
{
    if (!DataSlots.IsValidIndex(SlotIndex))
    {
        EMBER_LOG(LogEmberItem, Warning, TEXT("RemoveItemFromSlot: Index is not valid."));
        return 0;
    }
    FInstancedStruct& SlotInstance = DataSlots[SlotIndex];
    int32 RemoveAmount = 0;
    
    if (FEmberSlotData* Slot = SlotInstance.GetMutablePtr<FEmberSlotData>())
    {
        if (Slot->bIsEmpty())
        {
            EMBER_LOG(LogEmberItem, Warning, TEXT("RemoveItemFromSlot: Slot is Empty."));
            return 0;
        }

        RemoveAmount = FMath::Min(QuantityToRemove, Slot->Quantity);
        Slot->Quantity -= RemoveAmount;
        
        FTotalItemInfo& Total = TotalData.FindOrAdd(Slot->ItemID);
        Total.RemoveItemQuantity(RemoveAmount);

        if (Slot->Quantity <= 0)
        {
            Slot->Clear();
            Total.RemoveItemIndexes(SlotIndex);
        }
        
        OnDataChangedDelegate.Broadcast(SlotIndex, DataSlots[SlotIndex]);
        
    }

         
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("ReturnAmount : %d"), RemoveAmount);
#endif
   
    return RemoveAmount;
}

void UEmberDataContainer::UseItemInSlot_Implementation(int32 SlotIndex)
{
    if (!DataSlots.IsValidIndex(SlotIndex))
    {
        EMBER_LOG(LogEmberItem, Warning, TEXT("UseItemInSlot: non Invalid slot index %d"), SlotIndex);
        return;
    }

    int32 UseQuantity = 0;
    FInstancedStruct& SlotInstance = DataSlots[SlotIndex];
    if (FEmberSlotData* Slot = SlotInstance.GetMutablePtr<FEmberSlotData>())
    {
        if (Slot->bIsEmpty())
        {
            if (Slot->ConsumableData.IsSet())
            {
                FDataTableRowHandle ConsumeHandle = Slot->ConsumableData.GetValue();

                if (const FConsumableInfoRow* ConsumeData = ConsumeHandle.GetRow<FConsumableInfoRow>(TEXT("UseItemInSlot_GetConsumeData")))
                {
                    HandleItemConsumption(ConsumeData);
                    UseQuantity = RemoveItemFromSlot_Implementation(SlotIndex, ConsumeData->ConsumeAmount);
                }
                else
                {
                    EMBER_LOG(LogEmberItem, Error, TEXT("UseItemInSlot: ConsumableComponentHandle for item %s in slot %d pointed to an invalid row or DataTable."), *Slot->ItemID.ToString(), SlotIndex);
                }
            }
            else
            {
                EMBER_LOG(LogEmberItem, Log, TEXT("UseItemInSlot: Item %s in slot %d is not consumable (no ConsumableComponentHandle)."), *Slot->ItemID.ToString(), SlotIndex);
                // 다른 유형의 아이템 사용 로직 (예: 장착 요청)
            }
        }
    }
        
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("UseAmount : %d"), UseQuantity);
#endif
   
}

int32 UEmberDataContainer::GetSlotMaxRow_Implementation() const
{
    return SlotMaxRow;
}

void UEmberDataContainer::MovedInItemByAnotherProvider(int32 IndexTo,
    TScriptInterface<UEmberSlotDataProviderInterface> AnotherProvider, int32 IndexFrom, int32 Quantity)
{
    FName ItemID = IEmberSlotDataProviderInterface::Execute_GetSlotItemID(AnotherProvider.GetObject(), IndexFrom);
    int32 AddItem = AddItem_Implementation(FItemPair(ItemID, Quantity), IndexTo);
    IEmberSlotDataProviderInterface::Execute_RemoveItemFromSlot(AnotherProvider.GetObject(), IndexFrom, AddItem);

      
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("End Moved"));
#endif
   
}

FName UEmberDataContainer::GetSlotItemID_Implementation(int32 InIndex) const
{
    FName ItemID = FName();
    if (DataSlots.IsValidIndex(InIndex))
    {
        const FInstancedStruct& SlotInstance = DataSlots[InIndex];
        if (const FEmberSlotData* Slot = SlotInstance.GetPtr<FEmberSlotData>())
        {
            ItemID = Slot->ItemID;
        }
    }
    
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("Return ID : %s"), *ItemID.ToString());
#endif
   
    return ItemID;
}

void UEmberDataContainer::MoveItemByWidget_Implementation(const FGameplayTag& InSlotTag, int32 IndexTo,
    const TScriptInterface<UEmberSlotDataProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity)
{
    if (AnotherProvider.GetObject() == this)
    {
        MoveItemByIndex(IndexTo, IndexFrom, Quantity);
    }
    else
    {
        MovedInItemByAnotherProvider(IndexTo, AnotherProvider, IndexFrom, Quantity);
    }
    
#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("End MoveItme"));
#endif
   
}


FInstancedStruct UEmberDataContainer::GetSlotDataByIndex(int32 InSlotIndex) const
{
    if (DataSlots.IsValidIndex(InSlotIndex))
    {
        return DataSlots[InSlotIndex];
    }
    
    return FInstancedStruct();
}

TMap<FName, int32> UEmberDataContainer::GetAllItemInfos_Implementation()
{
    TMap<FName, int32> Items;

    for (auto& TotalItemInfo : TotalData)
    {
        Items.FindOrAdd(TotalItemInfo.Key) += TotalItemInfo.Value.TotalQuantity;
    }
    return Items;
}

bool UEmberDataContainer::bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems)
{
    for (const FItemPair& Item : InRequireItems)
    {
        if (Item.ItemID.IsValid() && Item.Quantity > 0)
        {
            if (const FTotalItemInfo* HasItem = TotalData.Find(Item.ItemID))
            {
                if (HasItem->TotalQuantity < Item.Quantity)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }
    
    return true;
}

void UEmberDataContainer::TryConsumeResource_Implementation(TArray<FItemPair>& InRequireItems)
{
    if (bConsumeAbleResource_Implementation(InRequireItems))
    {
        for (const FItemPair& Item : InRequireItems)
        {
            RemoveItemAutomatic(Item);
        }
    }
}

TArray<FItemPair> UEmberDataContainer::RemoveResourceUntilAble_Implementation(const TArray<FItemPair>& InRequireItems)
{
    TArray<FItemPair> RemainingRequireItems;
    for (const FItemPair& RequireItem : InRequireItems)
    {
        int32 RemovedQuantity = RemoveItemAutomatic(RequireItem);
        int RequiredQuantity = RequireItem.Quantity - RemovedQuantity;
        RequiredQuantity = RequiredQuantity > 0 ? RequiredQuantity : 0;
        if (RequireItem.Quantity - RemovedQuantity > 0)
        {
            RemainingRequireItems.Add(FItemPair(RequireItem.ItemID, RequiredQuantity));
        }
    }
    return RemainingRequireItems;
}


int32 UEmberDataContainer::FindEmptyFirstSlot() const
{
    int32 FirstEmptySlotIndex = INDEX_NONE;
    for (int32 Index = 0; Index < DataSlots.Num(); ++Index)
    {
        const FInstancedStruct& SlotInstance = DataSlots[Index];
        if (const FEmberSlotData* Slot = SlotInstance.GetPtr<FEmberSlotData>())
        {
            if (Slot->bIsEmpty())
            {
                FirstEmptySlotIndex = Index;
                break;
            }
        }
    }

#if UE_BUILD_DEVELOPMENT
    EMBER_LOG(LogEmberItem, Display, TEXT("FirstSlotIndex : %d"), FirstEmptySlotIndex);
#endif
    
    return FirstEmptySlotIndex;
}


void UEmberDataContainer::HandleItemConsumption(const FConsumableInfoRow* ConsumeData)
{
    if (!ConsumeData || !OwnerAbilitySystemComponent) return;

    UItemSystemLibrary::ApplyEffectInfoList(OwnerAbilitySystemComponent, ConsumeData->EffectsToApplyOnConsume, Owner); 
}
