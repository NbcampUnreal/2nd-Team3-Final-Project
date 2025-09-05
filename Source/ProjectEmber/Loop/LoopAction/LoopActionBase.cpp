// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/LoopAction/LoopActionBase.h"

#include "Fragment/LoopActionFragment.h"
#include "Kismet/GameplayStatics.h"
#include "Loop/LoopEventDirector.h"

void ULoopActionBase::ActivateAction_Implementation()
{
}

void ULoopActionBase::Initialize(ULoopEventDirector* InDirector, ULoopActionDefinition* InDefinition)
{
	OwnerDirector = InDirector;
	Definition = InDefinition;
}

void ULoopActionBase::FinishAction()
{
	if (OwnerDirector)
	{
		OwnerDirector->OnActionFinished(this);
	}
}

ACharacter* ULoopActionBase::GetPlayerCharacterFromWorld()
{
	UWorld* World = OwnerDirector->GetWorld();
	if (!World) return nullptr;
	return World ? UGameplayStatics::GetPlayerCharacter(World, 0) : nullptr;
}

ULoopActionFragment* ULoopActionBase::FindFragmentByClass(TSubclassOf<ULoopActionFragment> FragmentClass) const
{
	for (ULoopActionFragment* Frag : Definition->Fragments)
	{
		if (Frag && Frag->IsA(FragmentClass))
		{
			return Frag;
		}
	}
	return nullptr;
}
