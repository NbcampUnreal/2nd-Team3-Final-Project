// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_WeaponTrail.h"

#include "NiagaraComponent.h"
#include "Character/EmberCharacter.h"

void UAnimNotifyState_WeaponTrail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (!MeshComp)
	{
		return;
	}
	
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	const AEmberCharacter* EmberChar = Cast<AEmberCharacter>(Owner);
	if (!EmberChar)
	{
		return;
	}
	
	if (bIsDualWeapon)
	{
		if (UNiagaraComponent* TrailComp = EmberChar->GetWeaponTrailComponent())
		{
			TrailComp->Activate(true);
		}
		if (UNiagaraComponent* DualTrailComp = EmberChar->GetDualWeaponTrailComponent())
		{
			DualTrailComp->Activate(true);
		}
	}
	else if (bIsOnlyRightWeapon)
	{
		if (UNiagaraComponent* TrailComp = EmberChar->GetWeaponTrailComponent())
		{
			TrailComp->Activate(true);
		}
	}
	else
	{
		if (UNiagaraComponent* DualTrailComp = EmberChar->GetDualWeaponTrailComponent())
		{
			DualTrailComp->Activate(true);
		}
	}
}

void UAnimNotifyState_WeaponTrail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	const AEmberCharacter* EmberChar = Cast<AEmberCharacter>(Owner);
	if (!EmberChar)
	{
		return;
	}
	
	if (UNiagaraComponent* TrailComp = EmberChar->GetWeaponTrailComponent())
	{
		TrailComp->Deactivate();
	}

	if (UNiagaraComponent* DualTrailComp = EmberChar->GetDualWeaponTrailComponent())
	{
		DualTrailComp->Deactivate();
	}	
}
