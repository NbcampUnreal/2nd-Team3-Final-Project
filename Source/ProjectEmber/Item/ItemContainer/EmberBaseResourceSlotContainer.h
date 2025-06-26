// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberBaseSlotContainer.h"
#include "Item/Craft/EmberResourceProvider.h"
#include "EmberBaseResourceSlotContainer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberBaseResourceSlotContainer : public UEmberBaseSlotContainer, public IEmberResourceProvider
{
	GENERATED_BODY()
public:

	virtual TMap<FName, int32> GetAllItemInfos_Implementation() override;
	virtual void GetItemInfo_Implementation(FEmberItemEntry& InItemEntry, FInstancedStruct& OutItemInfo) override;
	virtual void GetItemInfos_Implementation(TArray<FEmberItemEntry>& InItemEntry, TMap<FEmberItemKey, FInstancedStruct>& OutItemInfos) override;
	virtual void TryConsumeResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems) override;
	virtual void RemoveResourceUntilAble_Implementation(TArray<FEmberItemEntry>& InRequireItems) override;
	virtual bool bConsumeAbleResource_Implementation(const TArray<FEmberItemEntry>& InRequireItems) override;

protected:
	virtual int32 RemoveItemAutomatic(const FEmberItemEntry& InItem);

	virtual int32 FindRemoveItemIndex(const FEmberItemEntry& InItemEntry);
};
