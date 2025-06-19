// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberItemCollectorBoxCollision.h"
#include "EmberResourceProvider.h"
#include "Components/ActorComponent.h"
#include "Item/UI/SlotWidget/EmberSlotDataProviderInterface.h"
#include "EmberCraftComponent.generated.h"


struct FCraftInfoRow;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberCraftComponent : public UEmberItemCollectorBoxCollision, public IEmberSlotDataProviderInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberCraftComponent();

	template<typename T>
	T* CraftItem(const FName& InItemID);
	
	UFUNCTION(BlueprintCallable)
	void CreateCraftItem(const FName& InItemID);
	
	UFUNCTION(BlueprintCallable)
	FEmberSlotData GetStoredItem();
	
	UFUNCTION(BlueprintCallable)
	bool bAbleCrafting(const FName& InItemID);

	UFUNCTION(BlueprintCallable)
	TArray<FCraftInfoRow> CraftItemInfos() const;

	// -- IEmberSlotDataProviderInterface --
	virtual FName GetSlotItemID_Implementation(int32 InIndex) const override;
	virtual FInstancedStruct GetSlotItemInfo_Implementation(int32 InIndex) const override;
	virtual int32 RemoveItemFromSlot_Implementation(int32 SlotIndex, int32 QuantityToRemove) override;

	
protected:
	UFUNCTION(BlueprintCallable, Category="Craft")
	FInstancedStruct CraftItemBase(const FName& InItemID);
	UFUNCTION(BlueprintCallable, Category="Craft")
	void RemoveStoredItem(int32& InQuantity);
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft|DataTables")
	TObjectPtr<UDataTable> CraftDataTable = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Craft|DataTables")
	FEmberSlotData StoredItem = FEmberSlotData();
	
};

template <typename T>
T* UEmberCraftComponent::CraftItem(const FName& InItemID)
{
	FInstancedStruct InstancedStruct = CraftItemBase(InItemID);
	return InstancedStruct.GetMutablePtr<T>();
}
