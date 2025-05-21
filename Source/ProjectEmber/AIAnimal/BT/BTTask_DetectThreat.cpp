// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_DetectThreat.h"

#include "AIController.h"
#include "AIAnimal/BaseAIAnimal.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_DetectThreat::UBTTask_DetectThreat()
{
	NodeName = "LeaderDetectThreat";
}

EBTNodeResult::Type UBTTask_DetectThreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
// 리더와 경비원 모두 위협을 감지하면 일정 시간 경계대상을 주시-> 컨트롤러에서 타이머로 처리
// 주시하는 동안 -> 이 노드에 들어오면  -> 위협대상의 위치를 기반으로 현재 자신의 위치에서 랜덤 방향, 랜덤 거리만큼 이동
//일정시간이 지나면 위협대상이 경계범위 안에 있어도 일단 idle 상태로 잠시 돌아감 -> 컨트롤러에서 타이머로 처리 계속 주시하고 있으면 부자연스럽기 때문에 중간중간 포커스를 의도적으로 삭제해줄 예정
//포커스는 타겟이고 이동은 랜덤인데 자연스럽게 보일지는 모르겠음-> 이상하면 수정방법 생각해봐야함
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	
	// 어슬렁거리기 로직 (간단하게 구현)
		FVector TargetActorLocation = TargetActor->GetActorLocation();
		FVector DirToThreat = (TargetActorLocation - AIPawn->GetActorLocation()).GetSafeNormal(); // 타겟향하는방향
		FVector RightVector = FVector::CrossProduct(DirToThreat, FVector::UpVector);
		FVector Offset = RightVector * FMath::RandRange(-150.f, 150.f) + DirToThreat * FMath::RandRange(-500.f, 500.f); //  좌우랜덤, 거리랜덤

		FVector MoveLocation = AIPawn->GetActorLocation() + Offset; // 위치+방향*크기
		AIController->MoveToLocation(MoveLocation, 10.f);

	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
