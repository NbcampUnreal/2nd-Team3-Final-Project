#include "GameplayEventSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UGameplayEventSubsystem::BroadcastGameEvent(FGameplayTag EventTag, const FGameplayEventData& EventData)
{
	OnGameEvent.Broadcast(EventTag, EventData);
}

UGameplayEventSubsystem* UGameplayEventSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}
	
	if (const UWorld* World = WorldContextObject->GetWorld())
	{
		if (const UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UGameplayEventSubsystem>();
		}
	}
	
	return nullptr;
}