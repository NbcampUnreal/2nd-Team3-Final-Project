#include "ProjectEmber/AIAnimal/BT/BTTask_Interact.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AIAnimal/TestFood.h"
#include "GameFramework/Character.h"
//#include "Interactables/BaseInteractableActor.h"
#include "Interactables/Interactable.h"
#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

UBTTask_Interact::UBTTask_Interact()
{
	NodeName = TEXT("Interact");
}

EBTNodeResult::Type UBTTask_Interact::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (TargetActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		ABaseAIAnimal* AICharacter = Cast<ABaseAIAnimal>(AIPawn);
		if (!AICharacter)
		{
			return EBTNodeResult::Failed;
		}
		
		AICharacter->PlayInteractMontage(static_cast<uint8>(EAnimalAIState::Idle)); //enum을 넘기고는 있지만 아직 사용하지 않음
		AICharacter->SetFullness();
		BlackboardComp->SetValueAsBool("IsHungry", false);
		Cast<ATestFood>(TargetActor)->Interact_Implementation(AICharacter); //먹이 삭제
		BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), nullptr);
		
		// 태스크 성공 반환
		return EBTNodeResult::Succeeded;
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
