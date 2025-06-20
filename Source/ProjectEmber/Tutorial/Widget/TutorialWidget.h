// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Tutorial/DataAssest/TutorialDataAsset.h"
#include "TutorialWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void SetImageTexture(UTexture2D* NewImage);

    UFUNCTION(BlueprintCallable)
    void ShowTutorial(bool bShow);

    UFUNCTION(BlueprintCallable)
    void SetTutorialData(const FTutorialData& InData);

    UFUNCTION()
    void OnCloseButtonClicked();
	
    UPROPERTY(meta = (BindWidget))
    class UImage* TutorialImage;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TutorialNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TutorialDescriptionText;

    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* BoderAnimation;

    UPROPERTY(meta = (BindWidget))
    class UButton* TutorialButton;
};