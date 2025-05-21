// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenuWidget.h"

#include "Character/EmberCharacter.h"

void UGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(PC->GetCharacter()))
		{
			EmberCharacter->BindUIInput(this);
		}
	}
}
