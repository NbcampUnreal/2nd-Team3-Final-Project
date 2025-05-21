// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GeneratePatrolArea.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_GeneratePatrolArea : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_GeneratePatrolArea();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	FVector GenerateRandomLocation(const FVector& BaseLocation, float Range);
};
