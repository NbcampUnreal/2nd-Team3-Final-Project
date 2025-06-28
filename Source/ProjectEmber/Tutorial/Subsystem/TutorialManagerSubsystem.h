// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tutorial/DataAssest/TutorialDataAsset.h"
#include "TutorialManagerSubsystem.generated.h"

class UTutorialWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTutorialTriggeredSignature, const FTutorialData&, TutorialData);

UCLASS(Config = Engine)
class PROJECTEMBER_API UTutorialManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UTutorialManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION()
	void HandleTutorialTriggered(const FTutorialData& TutorialData);

	void ShowTutorialSequence(const TArray<FTutorialData>& Sequence);
	void OnNextTutorialRequested();
	void HideTutorial();
	
	UPROPERTY(BlueprintAssignable) // 블루프린트에서도 바인딩 가능하게
	FTutorialTriggeredSignature OnTutorialTriggered;
	void ShowTutorial(const FTutorialData& TutorialData);
private:
	UPROPERTY()
	TSubclassOf<UUserWidget> TutorialWidgetClass;

	UPROPERTY()
	UTutorialWidget* TutorialWidget;

	TArray<FTutorialData> CurrentTutorialSequence;
	int32 CurrentTutorialIndex = 0;
	bool bIsTutorialActive = false;
};