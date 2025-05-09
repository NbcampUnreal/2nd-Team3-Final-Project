#include "Interactables/InteractableMover.h"

AInteractableMover::AInteractableMover()
{
	MoveCurve = LoadObject<UCurveFloat>(nullptr, TEXT(""));
	
	FOnTimelineFloat Progress;
	Progress.BindUFunction(this, FName("TimelineProgress"));
	MoveTimeline.AddInterpFloat(MoveCurve, Progress);
}

void AInteractableMover::Interact_Implementation(AActor* Caller)
{
	
}

float AInteractableMover::GetGatherTime_Implementation()
{
	return 0.0f;
}
