// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSystemLibrary.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "EmberLog/EmberLog.h"
#include "Engine/GameInstance.h"
#include "Item/ItemSubsystem.h"

UGameInstance* UItemSystemLibrary::GetGameInstance()
{
	if (GEngine && GEngine->GameViewport)
	{
		return GEngine->GameViewport->GetGameInstance();
	}
	return nullptr;
}

UItemSubsystem* UItemSystemLibrary::GetItemSubsystem()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		return GameInstance->GetSubsystem<UItemSubsystem>();
	}
	return nullptr;
}

void UItemSystemLibrary::ApplyEffectInfoList(UAbilitySystemComponent* TargetASC,
	const TArray<FItemEffectApplicationInfo>& EffectInfos, UObject* SourceObject, float SourceLevel)
{

	if (!TargetASC || EffectInfos.Num() == 0)
	{
		return;
	}

	for (const FItemEffectApplicationInfo& EffectInfo : EffectInfos)
	{

		if (!EffectInfo.GameplayEffectClass) continue;

		FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
		if (SourceObject)
		{
			ContextHandle.AddSourceObject(SourceObject);
		}

		FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(EffectInfo.GameplayEffectClass, SourceLevel, ContextHandle);
		if (SpecHandle.IsValid())
		{
			if (EffectInfo.MagnitudeSetByCallerTag.IsValid())
			{
				SpecHandle.Data->SetSetByCallerMagnitude(EffectInfo.MagnitudeSetByCallerTag, EffectInfo.Magnitude);
			}
			
			TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}
