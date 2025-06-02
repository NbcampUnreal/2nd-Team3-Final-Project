#pragma once

#include "CoreMinimal.h"
#include "EnumSet.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "Character/InputHandler/EmberKeyMappingEntry.h"
#include "InputMappingContext.h"
#include "UObject/SoftObjectPtr.h"
#include "InputTriggers.h"
#include "InputModifiers.h"
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

	UFUNCTION(BlueprintCallable)
	void TestPlaySFX(ESfxSoundType SoundType, const FName RowName, FVector Location = FVector::ZeroVector);
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FEmberKeyMappingEntry> SavedMappings;

	UFUNCTION(BlueprintCallable)
	void ApplySavedMoveBindingsToUserSettings();

	UPROPERTY(BlueprintReadWrite)
	TArray<FEmberDirectionalMoveEntry> SavedMoveBindings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> UI_ALS_MappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> UIMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

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
