// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment_Dialogue.h"

#include "AI_NPC/NPC_Component/DialogueComponent.h"
#include "Character/InputHandler/EmberInputHandlerComponent.h"
#include "Kismet/GameplayStatics.h"

bool UInteractionFragment_Dialogue::CanExecuteInteraction_Implementation(AActor* Interactor) const
{
	if (!Interactor) return false;
	
	UDialogueComponent* LocalDialogueComponent = Cast<UDialogueComponent>(GetOwner()->GetComponentByClass(UDialogueComponent::StaticClass()));
	UEmberInputHandlerComponent* LocalInputHandlerComponent = Cast<UEmberInputHandlerComponent>(Interactor->GetComponentByClass(UEmberInputHandlerComponent::StaticClass()));
	return (LocalDialogueComponent && LocalInputHandlerComponent);
}

void UInteractionFragment_Dialogue::ExecuteInteraction_Implementation(AActor* Interactor)
{
	DialogueComponent = Cast<UDialogueComponent>(GetOwner()->GetComponentByClass(UDialogueComponent::StaticClass()));
	InputHandlerComponent = Cast<UEmberInputHandlerComponent>(Interactor->GetComponentByClass(UEmberInputHandlerComponent::StaticClass()));

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PC) return;

	// PlayerController가 가진 InputComponent는 보통 nullptr이므로, Pawn에서 가져오는 게 안전
	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) return;

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerPawn->InputComponent);
	if (!EnhancedInput)
	{
		UE_LOG(LogTemp, Error, TEXT("EnhancedInputComponent가 필요합니다."));
		return;
	}
	
	if (InputHandlerComponent->InteractAction)
	{
		//auto Bind = [&](UInputAction* Action, ETriggerEvent Trigger, auto Func)
		//{
		//	if (IsValid(Action))
		//	{
		//		EnhancedInput->BindAction(Action, Trigger, DialogueComponent, Func);
		//	}
		//	
		//};
		EnhancedInput->BindAction(
						InputHandlerComponent->InteractAction.Get(),
						ETriggerEvent::Started,
						DialogueComponent,
						FName(TEXT("AdvanceDialogue"))
					);
		//EnhancedInput->BindAction(InputHandlerComponent->InteractAction.Get(), ETriggerEvent::Started, DialogueComponent, &UDialogueComponent::AdvanceDialogue);
		

		//Bind(InputHandlerComponent->InteractAction, ETriggerEvent::Started, &UDialogueComponent::AdvanceDialogue);
	}
	
	DialogueComponent->Interact();
}

void UInteractionFragment_Dialogue::EndInteraction_Implementation()
{
	// 바인딩 해제
	
}
