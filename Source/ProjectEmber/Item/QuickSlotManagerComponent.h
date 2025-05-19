#pragma once

#include "CoreMinimal.h"
#include "InventoryManagerComponent.h"
#include "QuickSlotManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuickSlotChangedDelegate, int32, SlotIndex,	const FQuickSlotData&, SlotData);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UQuickSlotManagerComponent : public UInventoryManagerComponent
{
	GENERATED_BODY()

public:
	UQuickSlotManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintAssignable, Category="QuickSlot")
	FOnQuickSlotChangedDelegate OnQuickSlotChanged;


	
};
