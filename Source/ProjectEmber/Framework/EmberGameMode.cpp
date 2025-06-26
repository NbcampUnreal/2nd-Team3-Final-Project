// EmberGameMode.cpp
#include "EmberGameMode.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlineAchievementsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

void AEmberGameMode::BeginPlay()
{
    Super::BeginPlay();
    
}

void AEmberGameMode::OnAchievementsWritten(const FUniqueNetId& PlayerId, bool bSuccess)
{
    
}
