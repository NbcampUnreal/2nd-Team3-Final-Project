// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Fragments/InteractionFragment_FollowSpline.h"

#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"


UInteractionFragment_FollowSpline::UInteractionFragment_FollowSpline()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionFragment_FollowSpline::BeginPlay()
{
	Super::BeginPlay();

	if (Curve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("TimelineProgress"));

		FOnTimelineEvent FinishedFunction;
		FinishedFunction.BindUFunction(this, FName("TimelineFinished"));

		Timeline.AddInterpFloat(Curve, ProgressFunction);
		Timeline.SetTimelineFinishedFunc(FinishedFunction);
		Timeline.SetLooping(false);
	}
}

void UInteractionFragment_FollowSpline::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Timeline.TickTimeline(DeltaTime);
}

void UInteractionFragment_FollowSpline::ExecuteInteraction_Implementation(AActor* Interactor)
{
	Super::ExecuteInteraction_Implementation(Interactor);

	TryMove();
}

bool UInteractionFragment_FollowSpline::TryMove()
{
	if (bIsPlaying) return false;

	if (!CalculateNextSegment(StartDistance, EndDistance)) return false;

	Move();
	return true;
}

bool UInteractionFragment_FollowSpline::CalculateNextSegment_Implementation(float& OutStartDistance,
                                                                            float& OutEndDistance)
{
	USplineComponent* Spline = MovementTarget.ResolveSpline(this);
	if (!Spline) return false;

	int NumPoints = Spline->GetNumberOfSplinePoints();
	if (CurrentSplineIndex + 1 >= NumPoints) return false;

	OutStartDistance = Spline->GetDistanceAlongSplineAtSplinePoint(CurrentSplineIndex);
	OutEndDistance = Spline->GetDistanceAlongSplineAtSplinePoint(CurrentSplineIndex + 1);

	return true;
}

void UInteractionFragment_FollowSpline::TimelineProgress(const float Value) const
{
	//UE_LOG(LogTemp, Warning, TEXT("TimelineProgress Value: %f"), Value);
	USplineComponent* Spline = MovementTarget.ResolveSpline(this);
	AActor* Target = MovementTarget.ResolveActor(this);

	if (!Spline || !Target) return;
	
	float Distance = FMath::Lerp(StartDistance, EndDistance, Value);
	UE_LOG(LogTemp, Warning, TEXT("TimelineProgress: Value=%.3f, Distance=%.3f"), Value, Distance);
	
	FVector Location = Spline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	
	if (bFollowSplineRotation)
	{
		FRotator Rotation = Spline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
		Target->SetActorLocationAndRotation(Location, Rotation);
	}
	else
	{
		Target->SetActorLocation(Location);
	}
}

void UInteractionFragment_FollowSpline::TimelineFinished()
{
	CurrentSplineIndex++;
	bIsPlaying = false;

	USplineComponent* Spline = MovementTarget.ResolveSpline(this);
	if (!Spline) return;

	// 다음 인덱스가 없으면 끝까지 도달한 것
	if (CurrentSplineIndex + 1 >= Spline->GetNumberOfSplinePoints())
	{
		OnFollowSplineFinished.Broadcast();
	}
}

void UInteractionFragment_FollowSpline::Move_Implementation()
{
	bIsPlaying = true;
	
	Timeline.Stop();
	Timeline.SetPlaybackPosition(0.f, false);
	Timeline.SetPlayRate(PlayRate); // 실행 전 초기화
	Timeline.PlayFromStart(); //TimelineProgress 함수 호출됨
}
