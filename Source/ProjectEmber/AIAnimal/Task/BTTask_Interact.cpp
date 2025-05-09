#include "ProjectEmber/AIAnimal/Task/BTTask_Interact.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"
#include "ProjectEmber/AIAnimal/TestFood.h"

UBTTask_Interact::UBTTask_Interact()
{
	NodeName = TEXT("Interact");
}

EBTNodeResult::Type UBTTask_Interact::ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory)
{
	AAIController* AIController = Comp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = Comp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	UObject* TargetObject = BlackboardComp->GetValueAsObject("TargetActor");
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor)
	{
		return EBTNodeResult::Succeeded;
	}
	float Distance = FVector::Dist(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());
	if (Distance > InteractionDistance)
	{
		return EBTNodeResult::Failed; // 너무 멀리 있음
	}

	if (TargetActor->GetClass()->ImplementsInterface(UInteractiveObject::StaticClass()))
	{
		ABaseAIAnimal* AICharacter = Cast<ABaseAIAnimal>(AIPawn);
		if (!AICharacter)
		{
			return EBTNodeResult::Failed;
		}
		
		AICharacter->PlayInteractMontage(static_cast<uint8>(EAnimalAIState::FindFood));
		AICharacter->SetFullness();
		BlackboardComp->SetValueAsBool("IsHungry", false);
		IInteractiveObject::Execute_Interact(TargetActor, AICharacter); //먹이 삭제
		BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), nullptr);
		
		// 태스크 성공 반환
		return EBTNodeResult::Succeeded;
	}
	
	return Super::ExecuteTask(Comp, NodeMemory);
}
