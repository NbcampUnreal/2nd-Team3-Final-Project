// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/EmberCharacter.h"
#include "Components/Button.h"
#include "DeathScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UDeathScreenWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	
public:
	UFUNCTION()
	void OnRespawnClicked();

	void SetOwner(APawn* NewOwner);
	
	UPROPERTY(meta = (BindWidget))
	UButton* RespawnButton;

	UPROPERTY()
	TObjectPtr<AEmberCharacter> OwningCharacter;
};
