// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_DetectThreat.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_DetectThreat : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_DetectThreat();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
