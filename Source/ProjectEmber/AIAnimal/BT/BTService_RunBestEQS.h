// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RunBestEQS.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTService_RunBestEQS : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_RunBestEQS();

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override; //서비스가 시작될 때 한 번 호출
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override; //설정된 주기(Interval)마다 호출
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override; //서비스가 더 이상 실행되지 않을 때 호출 -> 노드전환

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EQS")
	TObjectPtr<class UEnvQuery> BestPointQuery;
};
