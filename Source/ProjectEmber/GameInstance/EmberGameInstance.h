#pragma once

#include "CoreMinimal.h"
#include "EnumSet.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "UObject/SoftObjectPtr.h"
#include "EmberGameInstance.generated.h"

class UAudioSubsystem;
class ULevelSubsystem;
class UUserWidget;

UCLASS()
class PROJECTEMBER_API UEmberGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void TestPlaySound();

	UFUNCTION(BlueprintCallable)
	void ShowLoadingScreen();

	UFUNCTION(BlueprintCallable)
	void HideLoadingScreen();

	UFUNCTION(BlueprintCallable)
	void RequestOpenLevel(FName MapName);

	// 현재 지역 확인용
	EAreaType CurrentAreaType = EAreaType::GrasslandArea;

private:
	UPROPERTY()
	UAudioSubsystem* AudioSubsystem;

	UPROPERTY()
	ULevelSubsystem* LevelSubsystem;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<UUserWidget> LoadingScreenClass;

	UPROPERTY()
	UUserWidget* LoadingScreenWidget;

	FStreamableManager AssetLoader;
};
