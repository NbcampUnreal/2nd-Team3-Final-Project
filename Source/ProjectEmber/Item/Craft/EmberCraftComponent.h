// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberItemCollectorBoxCollision.h"
#include "EmberResourceProvider.h"
#include "Components/ActorComponent.h"
#include "Item/UI/EmberSlotProviderInterface.h"
#include "EmberCraftComponent.generated.h"


class UEmberCraftContainer;
class UEmberBaseResourceSlotContainer;
struct FCraftInfoRow;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberCraftComponent : public UEmberItemCollectorBoxCollision, public IEmberSlotProviderInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberCraftComponent();

	virtual void BeginPlay() override;

	template<typename T>
	T* CraftItem(const FName& InItemID);
	
	UFUNCTION(BlueprintCallable)
	void CreateCraftItem(const FName& InItemID);
	
	UFUNCTION(BlueprintCallable)
	bool bAbleCrafting(const FName& InItemID);

	UFUNCTION(BlueprintCallable)
	TArray<FCraftInfoRow> BP_CraftItemInfos() const;
	
	TArray<FCraftInfoRow*> CraftItemInfos() const;
	
	FCraftInfoRow* CraftItemInfo(const int32 InIndex) const;
	FInstancedStruct ResultItemInfo(const int32 InIndex) const;
	
	TObjectPtr<UEmberCraftContainer> GetSlotContainer() const;
	
	// --- IEmberSlotProviderInterface ---
	virtual int32 GetSlotCount_Implementation() const override;
	virtual FInstancedStruct GetSlotItemInfo_Implementation(int32 InIndex) const override;
	virtual void AddItemByWidget_Implementation(FEmberItemEntry& InOutItem, int32 InSlotIndex = -1) override;
	virtual int32 RemoveItemFromSlot_Implementation(int32 InSlotIndex, int32 QuantityToRemove) override;
	virtual void MoveItemByWidget_Implementation(const FGameplayTag& SlotTag, int32 IndexTo, const TScriptInterface<UEmberSlotProviderInterface>& AnotherProvider, int32 IndexFrom, int32 Quantity = -1) override;
	virtual int32 GetSlotMaxRow_Implementation() const override;
	virtual void UseItemInSlot_Implementation(int32 SlotIndex) override;

protected:
	UFUNCTION(BlueprintCallable, Category="Craft")
	FInstancedStruct CraftItemBase(const FName& InItemID);
	
	UFUNCTION(BlueprintCallable, Category="Craft")
	void RemoveStoredItem(int32& InQuantity);
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft|DataTables")
	TObjectPtr<UDataTable> CraftDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft|DataTables", SaveGame, Instanced)
	TObjectPtr<UEmberCraftContainer> SlotContainer = nullptr;

	TArray<FCraftInfoRow*> CraftItems;

};

template <typename T>
T* UEmberCraftComponent::CraftItem(const FName& InItemID)
{
	FInstancedStruct InstancedStruct = CraftItemBase(InItemID);
	return InstancedStruct.GetMutablePtr<T>();
}
