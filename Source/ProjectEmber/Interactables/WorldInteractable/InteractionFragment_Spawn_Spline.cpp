// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/InteractionFragment_Spawn_Spline.h"

#include "Components/SplineComponent.h"
#include "EmberLog/EmberLog.h"

UInteractionFragment_Spawn_Spline::UInteractionFragment_Spawn_Spline()
{
	PrimaryComponentTick.bCanEverTick = false;
}



void UInteractionFragment_Spawn_Spline::OnRegister()
{
	Super::OnRegister();

	if (AActor* Owner = GetOwner())
	{
		TargetSpline = Owner->FindComponentByClass<USplineComponent>();
	}
}


void UInteractionFragment_Spawn_Spline::SpawnActors(UWorld* World)
{
	Super::SpawnActors(World);

	if (!World || !TargetSpline)
	{
		UE_LOG(LogEmberInteraction, Error, TEXT("UMiniGameFragment_Spawn_Spline::SpawnActors Failed"));
		return;
	}

	int32 NumPoints = TargetSpline->GetNumberOfSplinePoints();

	for (int32 i = 0; i < NumPoints; i++)
	{
		FVector SpawnLocation = TargetSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		FRotator SpawnRotation = TargetSpline->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World);
		SpawnRotation += FRotator(0.f, 90.f, 0.f); // 오프셋 원인 찾으면 지우기
		
		// 클래스 선택: SpawnList 안에서 순환 없으면 DefaultActorClass
		TSubclassOf<AActor> SpawnClass = SpawnList.Num() > 0
		? SpawnList[i % SpawnList.Num()]
		: DefaultActorClass;
		if (!SpawnClass) continue;

		FActorSpawnParameters SpawnParams;
		AActor* SpawnedActor = World->SpawnActor<AActor>(SpawnClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (IsValid(SpawnedActor))
		{
			SpawnedActor->SetOwner(GetOwner());
			SpawnedList.Add(SpawnedActor);
		}
	}
}

void UInteractionFragment_Spawn_Spline::DestroySpawnedActors()
{
	for (AActor* Actor : SpawnedList)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
	SpawnedList.Empty();
}