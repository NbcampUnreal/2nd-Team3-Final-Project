// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "Components/TimelineComponent.h"
#include "Interactables/Fragments/InteractionFragment.h"
#include "InteractionFragment_FollowSpline.generated.h"

class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFollowSplineFinished);

USTRUCT(BlueprintType)
struct FInteractionMovementTarget
{
	GENERATED_BODY()

	UPROPERTY(EditInstanceOnly, Category = "Target")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditInstanceOnly, Category = "Target")
	TObjectPtr<AActor> SplineTarget = nullptr;

	AActor* ResolveActor(const UObject* Context) const
	{
		if (TargetActor) return TargetActor;
		if (const AActor* Owner = Context ? Cast<AActor>(Context->GetOuter()) : nullptr)
		{
			return const_cast<AActor*>(Owner); // const 제거 필요
		}
		return nullptr;
	}

	USplineComponent* ResolveSpline(const UObject* Context) const
	{
		if (SplineTarget)
		{
			USplineComponent* Spline = SplineTarget->FindComponentByClass<USplineComponent>();
			if (Spline) return Spline;
		}
		
		if (const AActor* Owner = Context ? Cast<AActor>(Context->GetOuter()) : nullptr)
		{
			return Owner->FindComponentByClass<USplineComponent>();
		}
		
		return nullptr;
	}
};

UCLASS(Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent))
class PROJECTEMBER_API UInteractionFragment_FollowSpline : public UInteractionFragment
{
	GENERATED_BODY()
public:
	UInteractionFragment_FollowSpline();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void ExecuteInteraction_Implementation(AActor* Interactor) override;

	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	bool CalculateNextSegment(float& OutStartDistance, float& OutEndDistance);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	bool TryMove();
	
	UFUNCTION()
	void TimelineProgress(float Value) const;

	UFUNCTION()
	void TimelineFinished();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFollowSplineFinished OnFollowSplineFinished;

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Interaction")
	void Move();

	UPROPERTY(EditAnywhere, Category="Interaction")
	bool bFollowSplineRotation = true;
	
	UPROPERTY(EditInstanceOnly, Category = "Interaction")
	FInteractionMovementTarget MovementTarget;
	
	UPROPERTY(EditAnywhere, Category="Interaction")
	UCurveFloat* Curve;

	UPROPERTY(EditAnywhere, Category="Interaction")
	float PlayRate = 1.f;

	FTimeline Timeline;
	
	bool bIsPlaying = false;
	int CurrentSplineIndex = 0;
	float StartDistance = 0.0f;
	float EndDistance = 0.0f;
};
