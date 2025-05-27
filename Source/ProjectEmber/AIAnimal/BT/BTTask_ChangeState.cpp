// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeState.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

UBTTask_ChangeState::UBTTask_ChangeState()
{
	NodeName = TEXT("ChangeState");
}

EBTNodeResult::Type UBTTask_ChangeState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController =  OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp =	AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	
	BlackboardComp->SetValueAsName("NStateTag", "Animal.State.Idle");
	BlackboardComp->SetValueAsObject("TargetObject", nullptr);
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
