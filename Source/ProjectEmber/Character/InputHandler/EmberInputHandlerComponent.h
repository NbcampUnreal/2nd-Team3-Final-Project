// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EmberInputHandlerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UEmberInputHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmberInputHandlerComponent();

public:
	void BindInput(UEnhancedInputComponent* InputComponent);
	
	void RegisterMapping(APlayerController* PC, int32 Priority = 0, const FModifyContextOptions& Options = FModifyContextOptions());
	void UnregisterMapping(APlayerController* PC);

public: /* Base Input */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookMouseAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> LookAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SprintAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> WalkAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> CrouchAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> AimAction;
	
	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> RagdollAction;
	
	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> RollAction;
	
	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> RotationModeAction;
	
	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> ViewModeAction;
	
	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> SwitchShoulderAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category="Input") 
	TObjectPtr<UInputAction> InteractAction;

public: /* UI Input */
	/* Comment : UI does not always run when you press it */
	
	UPROPERTY(EditAnywhere, Category="Input|UI")
	TObjectPtr<UInputMappingContext> UIInputMappingContext;

	UPROPERTY(EditAnywhere, Category="Input|UI") 
	TObjectPtr<UInputAction> UIInventoryAction;
	
	UPROPERTY(EditAnywhere, Category="Input|UI") 
	TObjectPtr<UInputAction> UIQuestAction;

	UPROPERTY(EditAnywhere, Category="Input|UI") 
	TObjectPtr<UInputAction> UICraftingAction;

	UPROPERTY(EditAnywhere, Category="Input|UI") 
	TObjectPtr<UInputAction> UIMapAction;

	UPROPERTY(EditAnywhere, Category="Input|UI") 
	TObjectPtr<UInputAction> UISkillAction;

	UPROPERTY(EditAnywhere, Category="Input|UI") 
	TObjectPtr<UInputAction> UIPauseAction;
};
