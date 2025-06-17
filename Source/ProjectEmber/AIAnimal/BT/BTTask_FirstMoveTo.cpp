// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_FirstMoveTo.h"

#include "AIAnimal/BaseAIAnimal.h"
#include "AIAnimal/TokenRaidSubsystem.h"

UBTTask_FirstMoveTo::UBTTask_FirstMoveTo()
{
	NodeName = TEXT("FirstMoveTo");
	bNotifyTaskFinished = true; 
}

EBTNodeResult::Type UBTTask_FirstMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	return Result;
}

void UBTTask_FirstMoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
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
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_FirstMoveTo - OnTaskFinished 실행됨"));
		GetWorld()->GetGameInstance()->GetSubsystem<UTokenRaidSubsystem>()->OnFirstMovementComplete(Animal, bSuccess);
	}
}
