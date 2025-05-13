// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/Task/BTTask_ChaseTarget.h"

#include "AIController.h"
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

	TArray<AActor*> TargetActor;
	// 플레이어 태그를 가진 엑터를 타겟엑터로 등록 -> 플레이어 전용 코드
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TargetPlayerTag, TargetActor);
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
		UE_LOG(LogTemp, Warning, TEXT("UBTTask_ChaseTarget::ExecuteTask 업데이트 성공. %f, %f, %f"), ClosestObject->GetActorLocation().X, ClosestObject->GetActorLocation().Y, ClosestObject->GetActorLocation().Z );
		return Super::ExecuteTask(OwnerComp, NodeMemory);
	}
		
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
