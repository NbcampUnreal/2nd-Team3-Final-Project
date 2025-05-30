// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FollowLeader.generated.h"

namespace EEnvQueryStatus
{
	enum Type : int;
}

class UEnvQueryInstanceBlueprintWrapper;
class UEnvQuery;
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

	UFUNCTION(BlueprintCallable)
	void OnFindLeaderOrPatrolQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
										EEnvQueryStatus::Type QueryStatus);

	
protected:
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FGameplayTag LeaderTag;

	//EQS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS")
	TObjectPtr<UEnvQuery> LeaderOrPatrolQuery;

	UBlackboardComponent* BlackboardComp;
	UBehaviorTreeComponent* BTComp;
};
