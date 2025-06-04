#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameplayEventSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnGameEvent,
	const FGameplayTag&, EventTag,
	const FGameplayEventData&, EventData
);

UCLASS()
class PROJECTEMBER_API UGameplayEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Gameplay Event", meta = (WorldContext = "WorldContextObject"))
	static UGameplayEventSubsystem* GetGameplayEvent(const UObject* WorldContextObject);
	
	UPROPERTY(BlueprintAssignable, Category = "Gameplay Event")
	FOnGameEvent OnGameEvent;
	
	UFUNCTION(BlueprintCallable, Category = "Gameplay Event")
	void BroadcastGameEvent(FGameplayTag EventTag, const FGameplayEventData& EventData);
	
};
