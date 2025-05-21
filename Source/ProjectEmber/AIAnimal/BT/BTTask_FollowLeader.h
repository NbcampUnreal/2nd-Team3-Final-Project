// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FollowLeader.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_FollowLeader : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_FollowLeader();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	FVector GenerateRandomLocation(const FVector& BaseLocation, float Range);

protected:
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FGameplayTag LeaderTag;
};
