// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindFood.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_FindFood : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_FindFood();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

protected:
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float SearchRadius = 30000.f;
	
	// 상호작용 가능한 오브젝트의 태그
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FName InteractiveObjectTag = "Interactive";
};
