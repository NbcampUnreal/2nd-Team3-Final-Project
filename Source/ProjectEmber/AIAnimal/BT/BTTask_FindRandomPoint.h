// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "BTTask_FindRandomPoint.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_FindRandomPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindRandomPoint();

	UFUNCTION()
	void OnFindRandomPointQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);
	
	//태스크 실행 로직
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	FVector GenerateRandomLocation(APawn* AIPawn, float Range);

protected:
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float SearchRadius = 3000.f;

	// 상호작용 가능한 오브젝트의 태그
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FName InteractiveObjectTag = "Interactive";

	//EQS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS")
	TObjectPtr<UEnvQuery> RandomPointQuery;

	UBlackboardComponent* BlackboardComp;
	UBehaviorTreeComponent* BTComp;
};
