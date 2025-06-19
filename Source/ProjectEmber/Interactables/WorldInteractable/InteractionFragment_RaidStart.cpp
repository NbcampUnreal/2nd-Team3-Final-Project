// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/InteractionFragment_RaidStart.h"

#include "AIAnimal/TokenRaidSubsystem.h"

void UInteractionFragment_RaidStart::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);
	OnStartRaid();
}

void UInteractionFragment_RaidStart::EndInteraction_Implementation()
{
	Super::EndInteraction_Implementation();
}

void UInteractionFragment_RaidStart::OnStartRaid()
{
	AAnimalSpawner* Owner = Cast<AAnimalSpawner>(GetOwner());
	if (Owner)
	{
		FTokenRaidInfo Row;
		Row = GetWorld()->GetGameInstance()->GetSubsystem<UTokenRaidSubsystem>()->
													GetRaidInfoRow(Owner->GetIdentityTag()); //테이블 정보 가져옴
		Owner->OnTokenRaidEvent(Row);
	}
}
