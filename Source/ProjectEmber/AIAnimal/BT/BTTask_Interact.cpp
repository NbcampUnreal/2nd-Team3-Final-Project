#include "ProjectEmber/AIAnimal/BT/BTTask_Interact.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AIAnimal/TestFood.h"
#include "GameFramework/Character.h"
//#include "Interactables/BaseInteractableActor.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
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

		//어빌리티에서 몽타주 재생 끝나는 시점에 바인딩된 함수 OnCompleteCallback 에서 처리하려고 했으나
		//모듈 깨짐 -> 직접 몽타주에 노티파이 만들어서 처리해야할 듯
		//AICharacter->SetFullness();
		//BlackboardComp->SetValueAsBool("IsHungry", false);
		//IInteractable::Execute_Interact(TargetActor,AICharacter); //먹이 삭제, testfood의 bp에 작성된 interact 호출
		//BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), nullptr);
		
		//여기서는 어빌리티 호출만 해줌, 어빌리티 트리거 이벤트 -> 몽타주 재생 -> 재생되는 몽타주에 노티파이 -> 노티파이에서 실제로 먹는게 다 끝날 때, 중간에 끊길 때 경우 처리
		FGameplayEventData Payload;
		Payload.EventTag = InteractTaskTag;
		Payload.Instigator = AICharacter;
		Payload.Target = TargetActor;
		Payload.OptionalObject = TargetActor;
		Payload.OptionalObject2 = AICharacter->GetMontage();

		// 게임플레이 이벤트를 어빌리티에게 전달(trigger)하는 함수, 어빌리티가 특정 GameplayTag 이벤트를 수신해서 발동되도록 하는 트리거 역할
		AICharacter->GetAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
		
		
		// 태스크 성공 반환
		return EBTNodeResult::Succeeded;
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
