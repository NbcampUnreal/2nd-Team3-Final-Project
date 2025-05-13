// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChaseTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_ChaseTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_ChaseTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FName TargetPlayerTag = "Player";
};
