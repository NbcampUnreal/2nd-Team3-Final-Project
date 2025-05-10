#include "Character/EmberComponents/InteractionComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInteractionComponent::SetCurrentInteractable(UObject* NewInteractable)
{
	CurrentInteractable = NewInteractable;
}

void UInteractionComponent::Interact()
{
	if (CurrentInteractable)
	{
		// 인터페이스 함수 호출
		IInteractable::Execute_Interact(CurrentInteractable.GetObject(), GetOwner());
	}
}
