// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SetPatrolAreaIndex.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_SetPatrolAreaIndex : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_SetPatrolAreaIndex();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
