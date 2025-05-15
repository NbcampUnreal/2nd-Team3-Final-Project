// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_HasInteractTag.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTDecorator_HasInteractTag : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTDecorator_HasInteractTag();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	FGameplayTag FoodTag;
};
