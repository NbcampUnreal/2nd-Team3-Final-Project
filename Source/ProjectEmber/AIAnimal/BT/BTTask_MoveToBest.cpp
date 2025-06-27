// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_MoveToBest.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "AIAnimal/TokenRaidSubsystem.h"

UBTTask_MoveToBest::UBTTask_MoveToBest()
{
	NodeName = TEXT("MoveToBest");
	bNotifyTaskFinished = true; 
}

EBTNodeResult::Type UBTTask_MoveToBest::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	AAIController* AIController = OwnerComp.GetAIOwner();
	ABaseAIAnimal* Animal = Cast<ABaseAIAnimal>(AIController->GetPawn());
	GetWorld()->GetGameInstance()->GetSubsystem<UTokenRaidSubsystem>()->ReturnToken(Animal); 
	return Result;
}

void UBTTask_MoveToBest::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	

	AAIController* AIController = OwnerComp.GetAIOwner();
	ABaseAIAnimal* Animal = Cast<ABaseAIAnimal>(AIController->GetPawn());
	
	if (IsValid(Animal))
	{
		bool bSuccess = false;
		if (EBTNodeResult::Succeeded == TaskResult)
		{
			bSuccess = true;
		}
		GetWorld()->GetGameInstance()->GetSubsystem<UTokenRaidSubsystem>()->OnMovementComplete(Animal, bSuccess);
	}
}
