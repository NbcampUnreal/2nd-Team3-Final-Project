#pragma once

#include "CoreMinimal.h"
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

private:
	UPROPERTY()
	UAudioSubsystem* AudioSubsystem;

	UPROPERTY()
	ULevelSubsystem* LevelSubsystem;
};
