#include "QuestConditionKillAnimal.h"

bool UQuestConditionKillAnimal::OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
{
	if (InEventTag != EventTag)
	{
		return false;
	}

	const AActor* TargetActor = EventData.Target.Get();
	if (!TargetActor)
	{
		return false;
	}

	// 액터 클래스가 TargetNPCClass와 일치하는지 확인
	if (TargetNPCClass.IsValid() && TargetActor->GetClass()->IsChildOf(TargetNPCClass.Get()))
	{
		CurrentCount++;
		return IsFulfilled();
	}

	return false;
}
FString UQuestConditionKillAnimal::GetConditionText_Implementation() const
{
	FString NameStr;
	if (ConditionName.IsEmpty())
	{
		NameStr = TargetNPCClass.IsValid() ? TargetNPCClass->GetName() : TEXT("UnknownTarget");
	}
	else
	{
		NameStr = ConditionName.ToString();
	}

	return FString::Printf(TEXT("%s: %d / %d"), *NameStr, CurrentCount, RequiredCount);
}