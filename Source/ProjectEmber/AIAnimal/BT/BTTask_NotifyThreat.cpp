// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_NotifyThreat.h"
#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"



UBTTask_NotifyThreat::UBTTask_NotifyThreat()
{
	NodeName = "NotifyThreat";
}

EBTNodeResult::Type UBTTask_NotifyThreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}


	
	// 리더가 먼저 위협을 감지한 경우 -> 게임플레이태그 Animal.State.Call, 
	//
	//리더가 경비를 호출

	
	//리더가 늦게 위협을 감지한 경우 -> Animal.State.Called
	//경비가  리더를 호출
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
