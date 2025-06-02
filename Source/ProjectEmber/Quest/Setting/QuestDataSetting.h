#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Quest/Data/QuestDataAsset.h"
#include "QuestDataSetting.generated.h"

UCLASS(config=Game, defaultconfig, meta=(DisplayName="Quest System"))
class PROJECTEMBER_API UQuestDataSetting : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category="Quest")
	TArray<TSoftObjectPtr<UQuestDataAsset>> QuestAssets;
};
