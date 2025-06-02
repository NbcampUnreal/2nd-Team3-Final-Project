// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Core/EmberItemStruct.h"
#include "Craft/EmberResourceProvider.h"
#include "UserItemManger.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UUserItemManger : public UActorComponent, public IEmberResourceProvider
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUserItemManger();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	FEmberItemInfo GetQuickSlotInfo(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void UseQuickSlotInfo(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	FEmberItemInfo GetInventorySlotInfo(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void UseInventorySlotInfo(int32 InIndex);

	UFUNCTION(BlueprintCallable)
	void GetMonsterItems(const FName MonsterID);
	/**
	 * 
	 * @param ItemID 넣을 아이템의 ID
	 * @param Quantity 넣을 수량
	 * @param InSlotIndex 넣을 슬롯을 지정합니다 범위가 벗어난경우 있는아이템이후 빈공간에 넣습니다
	 */
	UFUNCTION(BlueprintCallable)
	void AddItem(FName ItemID, int32 Quantity, int32 InSlotIndex = -1);
	
	const class UInventoryManager* GetInventoryManager() const;
	
	UInventoryManager* GetInventoryManager();
	
	const class UQuickSlotManager* GetQuickSlotManager() const;

	UQuickSlotManager* GetQuickSlotManager();
	
	const class UEmberEquipmentManager* GetEquipmentManager() const;

	UEmberEquipmentManager* GetEquipmentManager();
	
	const class UEmberDropItemManager* GetEmberDropItemManager() const;

	UEmberDropItemManager* GetEmberDropItemManager();

	// --- IEmberResourceProvider ---
	virtual TMap<FName, int32> GetAllItemInfos_Implementation() override;
	
	virtual void TryConsumeResource_Implementation(const TArray<FItemPair>& InRequireItems) override;

	virtual TArray<FItemPair> RemoveResourceUntilAble_Implementation(const TArray<FItemPair>& InRequireItems) override;

	virtual bool bConsumeAbleResource_Implementation(const TArray<FItemPair>& InRequireItems) override;


public: /* Quick Slot Interaction */
	FName SelectQuickSlot(int32 InIndex) const;
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	int32 InventoryMaxSlot = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	int32 InventoryMaxSlotRow = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	int32 QuickMaxSlot = 10;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	int32 QuickMaxSlotRow = 10;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	int32 DropItemMaxSlot = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	int32 DropItemMaxSlotRow = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Manager")
	TObjectPtr<UInventoryManager> InventoryManager;
	TObjectPtr<UQuickSlotManager> QuickSlotManager;
	TObjectPtr<UEmberEquipmentManager> EquipmentManager;
	TObjectPtr<UEmberDropItemManager> DropItemManager;
	
};
