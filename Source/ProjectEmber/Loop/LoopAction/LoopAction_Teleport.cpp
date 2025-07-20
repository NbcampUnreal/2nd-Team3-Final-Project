// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/LoopAction/LoopAction_Teleport.h"

#include "Character/RespawnTarget/RespawnTarget.h"
#include "Fragment/LoopActionFragment_Locations.h"
#include "Kismet/GameplayStatics.h"
#include "Loop/LoopEventDirector.h"

void ULoopAction_Teleport::ActivateAction_Implementation()
{
	Super::ActivateAction_Implementation();

	ULoopActionFragment_Locations* Locations = FindFragment<ULoopActionFragment_Locations>();
	
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerDirector->GetWorld(), 0);
	PlayerPawn->SetActorLocation(Locations->RespawnTarget->GetActorLocation());
	FinishAction();
}
