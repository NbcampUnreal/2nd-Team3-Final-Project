// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tutorial/DataAssest/TutorialDataAsset.h"
#include "TutorialManagerSubsystem.generated.h"

class UTutorialWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialTriggered, const FTutorialData&, TutorialData);

UCLASS(Config = Engine)
class PROJECTEMBER_API UTutorialManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    UTutorialManagerSubsystem();
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    UPROPERTY(BlueprintAssignable)
    FOnTutorialTriggered OnTutorialTriggered;

    UFUNCTION()
    void ShowTutorial(const FTutorialData& TutorialData);

    UFUNCTION(BlueprintCallable)
    void HideTutorial();

    UFUNCTION()
    void HandleTutorialTriggered(const FTutorialData& TutorialData);

private:
    TSubclassOf<UTutorialWidget> TutorialWidgetClass;
    UTutorialWidget* TutorialWidget = nullptr;
};