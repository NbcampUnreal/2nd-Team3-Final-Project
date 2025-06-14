#include "CombatFunctionLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UCombatFunctionLibrary::ApplyGlobalTimeDilation(UWorld* World, float TimeDilation, float Duration)
{
	if (!World)
	{
		return;
	}
	
	World->GetWorldSettings()->SetTimeDilation(TimeDilation);
	
	FTimerDelegate Delegate = FTimerDelegate::CreateStatic(&UCombatFunctionLibrary::ResetGlobalTimeDilation, World);
	
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, Delegate, Duration, false);
}

void UCombatFunctionLibrary::ResetGlobalTimeDilation(UWorld* World)
{
	if (!World)
	{
		return;
	}
	
	World->GetWorldSettings()->SetTimeDilation(1.0f);
}

void UCombatFunctionLibrary::ApplyActorTimeDilation(AActor* TargetActor, float TimeDilation, float Duration)
{
	if (!TargetActor)
	{
		return;
	}
	
	UWorld* World = TargetActor->GetWorld();
	if (!World)
	{
		return;
	}
	
	TargetActor->CustomTimeDilation = TimeDilation;
	
	FTimerDelegate Delegate = FTimerDelegate::CreateStatic(&UCombatFunctionLibrary::ResetActorTimeDilation, TargetActor);
	
	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, Delegate, Duration, false);
}

void UCombatFunctionLibrary::ResetActorTimeDilation(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}
	
	TargetActor->CustomTimeDilation = 1.0f;
}
