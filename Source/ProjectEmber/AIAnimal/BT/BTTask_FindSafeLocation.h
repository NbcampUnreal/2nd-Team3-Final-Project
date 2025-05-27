// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindSafeLocation.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;

namespace EEnvQueryStatus
{
	enum Type : int;
}

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_FindSafeLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindSafeLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	FVector GenerateRandomLocation(const FVector& TargetActorLocation, const FVector& ActorLocation);

	UFUNCTION(BlueprintCallable)
	void OnFindSafeLocationQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
										EEnvQueryStatus::Type QueryStatus);
	
	//EQS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS")
	TObjectPtr<UEnvQuery> SafeLocationQuery;

	UBlackboardComponent* BlackboardComp;
	UBehaviorTreeComponent* BTComp;
};
