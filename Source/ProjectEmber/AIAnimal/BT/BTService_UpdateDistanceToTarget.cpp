// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateDistanceToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_UpdateDistanceToTarget::UBTService_UpdateDistanceToTarget()
{
	NodeName = TEXT("Update Distance To Target");
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
}

void UBTService_UpdateDistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
                                                 float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}
	
	if (AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor")))
	{
// 		// 디버그 시각화 추가
// #if WITH_EDITOR
// 		DrawDebugLine(
// 			GetWorld(),
// 			 AIPawn->GetActorLocation(),
// 			Target->GetActorLocation(),
// 			FColor::Red,
// 			false,
// 			-1.0f,
// 			0,
// 			10.0f
// 		);
// #endif
		
		if (AIPawn && Target)
		{
			float Distance = FVector::Dist(AIPawn->GetActorLocation(), Target->GetActorLocation());
			BlackboardComp->SetValueAsFloat("DistanceToTarget", Distance);
			BlackboardComp->SetValueAsVector("TargetLocation", Target->GetActorLocation());
		}
	}
}
