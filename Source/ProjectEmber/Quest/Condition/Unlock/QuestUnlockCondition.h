#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestSubsystem.h"
#include "Quest/Condition/QuestCondition.h"
#include "QuestUnlockCondition.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UQuestUnlockCondition : public UQuestCondition
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
	FName PrerequisiteQuestID;
	
	virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData) override
	{
		if (IsFulfilled())
		{
			return true;
		}

		UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
		if (!GameInstance)
		{
			return false;
		}

		UQuestSubsystem* QuestSys = GameInstance->GetSubsystem<UQuestSubsystem>();
		if (!QuestSys)
		{
			return false;
		}
		
		if (QuestSys->IsQuestCompleted(PrerequisiteQuestID))
		{
			CurrentCount = RequiredCount;
			return true;
		}
		
		return false;
	}
};
