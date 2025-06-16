// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "BTTask_FirstMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_FirstMoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()
	
public:
	UBTTask_FirstMoveTo();

	//태스크 실행 로직
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

};
