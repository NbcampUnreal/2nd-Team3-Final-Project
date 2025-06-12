#include "ProjectEmber/AIAnimal/BT/BTTask_Interact.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
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
	
	UObject* TargetObject = BlackboardComp->GetValueAsObject("NTargetFood");
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}
	
		ABaseAIAnimal* AICharacter = Cast<ABaseAIAnimal>(AIPawn);
		if (!AICharacter)
		{
			return EBTNodeResult::Failed;
		}
		
		//어빌리티 트리거 이벤트 -> 몽타주 재생 -> 재생되는 몽타주에 노티파이 -> 노티파이에서 실제로 먹는게 다 끝날 때, 중간에 끊길 때 경우 처리
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag("Trigger.Animal.Interact.Harvest");
		Payload.Instigator = AICharacter;
		Payload.Target = TargetActor;
		Payload.OptionalObject = TargetActor;
		Payload.OptionalObject2 = AICharacter->GetMontage(FGameplayTag::RequestGameplayTag("Animal.Montage.Harvest"));

		// 게임플레이 이벤트를 어빌리티에게 전달(trigger)하는 함수, 어빌리티가 특정 GameplayTag 이벤트를 수신해서 발동되도록 하는 트리거 역할
		AICharacter->GetAbilitySystemComponent()->HandleGameplayEvent(Payload.EventTag, &Payload);
		
		
		// 태스크 성공 반환
		return EBTNodeResult::Succeeded;
	
}
