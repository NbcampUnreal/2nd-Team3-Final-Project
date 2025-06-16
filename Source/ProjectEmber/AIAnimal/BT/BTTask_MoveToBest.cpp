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
	UE_LOG(LogTemp, Warning, TEXT("ExecuteTask - MoveToBest 실행됨"));
	return Result;
}

void UBTTask_MoveToBest::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	

	AAIController* AIController = OwnerComp.GetAIOwner();
	ABaseAIAnimal* Animal = Cast<ABaseAIAnimal>(AIController->GetPawn());

	//베스트 포인트로 이동했다 -> 최초 공격대형 잡을 때(토큰 발행전 -> 알아서 리턴), 공격하고 돌아갈 때(돌아왓으면 토큰 반환)
	
	
	if (IsValid(Animal))
	{
		bool bSuccess = false;
		if (EBTNodeResult::Succeeded == TaskResult)
		{
			bSuccess = true;
		}
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_MoveToBest - OnTaskFinished 실행됨"));
		//Animal->OnAnimalMoveFinish.Broadcast(Cast<AActor>(Animal), bSuccess);
		GetWorld()->GetGameInstance()->GetSubsystem<UTokenRaidSubsystem>()->OnMovementComplete(Animal, bSuccess);
	}
}
