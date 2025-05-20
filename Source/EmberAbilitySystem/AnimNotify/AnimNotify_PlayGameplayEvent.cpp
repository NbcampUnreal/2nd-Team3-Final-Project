#include "AnimNotify_PlayGameplayEvent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"

void UAnimNotify_PlayGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!EventTag.IsValid() || !MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}
	
	UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(Owner->FindComponentByClass<UAbilitySystemComponent>());
	if (!ASC)
	{
		return;
	}
	
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = Owner;
	Payload.Target = Owner;
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner,	EventTag,Payload);
}
