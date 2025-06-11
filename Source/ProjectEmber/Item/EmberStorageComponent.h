// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Craft/EmberResourceProvider.h"
#include "EmberStorageComponent.generated.h"


class UInventoryManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberStorageComponent : public UActorComponent, public IEmberResourceProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEmberStorageComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	/**
		 * 
		 * @param ItemID 넣을 아이템의 ID
		 * @param Quantity 넣을 수량
		 * @param InSlotIndex 넣을 슬롯을 지정합니다 범위가 벗어난경우 있는아이템이후 빈공간에 넣습니다
		 */
	UFUNCTION(BlueprintCallable)
	void AddItem(FName ItemID, int32 Quantity, int32 InSlotIndex = -1);
	
	// --- IEmberResourceProvider ---
	virtual TMap<FName, int32> GetAllItemInfos_Implementation() override;
	
	virtual void TryConsumeResource_Implementation(const TArray<FItemPair>& InRequireItems) override;

	virtual void RemoveResourceUntilAble_Implementation(TArray<FItemPair>& InRequireItems) override;
	
	virtual bool bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInventoryManager> InventoryManager;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	int32 InventoryMaxSlot = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	int32 InventoryMaxSlotRow = 10;
	
};
