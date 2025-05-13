// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Interact.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_Interact : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_Interact();

	//태스크 실행 로직
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = "true"))
	float InteractionDistance = 50.0f;
};
