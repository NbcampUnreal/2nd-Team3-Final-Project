// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/LoopAction/LoopActionHelper.h"

#include "Kismet/GameplayStatics.h"
#include "UI/HUD/EmberMainHUD.h"

void ULoopActionHelper::SetSoundMixClassOverride(UObject* WorldContextObject, USoundMix* SoundMix, USoundClass* SoundClass,
                                                 float Volume, float Pitch, float FadeTime)
{
	if (!SoundMix || !SoundClass)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (!World)
	{
		return;
	}

	UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, Volume, Pitch, FadeTime);
}

void ULoopActionHelper::PushSoundMix(UObject* WorldContextObject, USoundMix* SoundMix)
{
	if (!SoundMix) return;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (!World) return;

	UGameplayStatics::PushSoundMixModifier(World, SoundMix);
}

void ULoopActionHelper::PopSoundMix(UObject* WorldContextObject, USoundMix* SoundMix)
{
	if (!SoundMix)
	{
		UE_LOG(LogTemp, Warning, TEXT("PopSoundMix: SoundMix is null"));
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("PopSoundMix: Invalid World Context"));
		return;
	}

	UGameplayStatics::PopSoundMixModifier(World, SoundMix);
}

void ULoopActionHelper::ClearSoundMixModifiers(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("PopSoundMix: Invalid World Context"));
		return;
	}
	UGameplayStatics::ClearSoundMixModifiers(World);
}

AEmberMainHUD* ULoopActionHelper::GetMainHUD(UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	APlayerController* PC = World->GetFirstPlayerController();
	return PC ? Cast<AEmberMainHUD>(PC->GetHUD()) : nullptr;
}
