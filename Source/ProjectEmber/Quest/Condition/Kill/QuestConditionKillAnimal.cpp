#include "QuestConditionKillAnimal.h"

UQuestConditionKillAnimal::UQuestConditionKillAnimal()
{
	// 블루프린트에서 세팅할 값
	//AnimalID = 소, 늑대, 곰, 사슴;

	
}

bool UQuestConditionKillAnimal::OnEvent_Implementation(const FGameplayTag& InEventTag,
	const FGameplayEventData& EventData)
{
	if (InEventTag != EventTag)
	{
		return false;
	}

	if (AiAnimal == EventData.Target.GetClass())
	{
		CurrentCount++;
		if (IsFulfilled())
		{
			return true;
		}
	}

	return false;
}


