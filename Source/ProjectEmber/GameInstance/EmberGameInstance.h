#pragma once

#include "CoreMinimal.h"
#include "EnumSet.h"
#include "Engine/GameInstance.h"
#include "EmberGameInstance.generated.h"

class UAudioSubsystem;
class ULevelSubsystem;

UCLASS()
class PROJECTEMBER_API UEmberGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void TestPlaySound();

	// 현재 지역 확인용
	EAreaType CurrentAreaType = EAreaType::GrasslandArea;

private:
	UPROPERTY()
	UAudioSubsystem* AudioSubsystem;

	UPROPERTY()
	ULevelSubsystem* LevelSubsystem;
};
