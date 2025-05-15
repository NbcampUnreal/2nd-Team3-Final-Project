// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_ChaseTarget.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_ChaseTarget::UBTTask_ChaseTarget()
{
	NodeName = TEXT("ChaseTarget");
}

EBTNodeResult::Type UBTTask_ChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	//회피기능은 전체적으로 버그 잡고 구현
	//맞았을 때 타겟 등록 , 서비스에서 
	TArray<AActor*> TargetActor;
	
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Player", TargetActor);
	AActor* ClosestObject = nullptr;
	for (auto Actor : TargetActor)
	{
		if (Actor)
		{
			ClosestObject = Actor;
		}
	}
	// 결과를 블랙보드에 저장
	if (ClosestObject)
	{
		BlackboardComp->SetValueAsObject("TargetActor", ClosestObject);
		BlackboardComp->SetValueAsVector("TargetLocation", ClosestObject->GetActorLocation());
	}
	
		
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
