// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EMSActorSaveInterface.h"
#include "GameFramework/PlayerController.h"
#include "EmberPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API AEmberPlayerController : public APlayerController, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	AEmberPlayerController();
	
	virtual void ActorPreSave_Implementation() override;
	virtual void ActorLoaded_Implementation() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Save|Thumbnail")
	void SaveThumbnailImage();

	UFUNCTION(BlueprintCallable, Category = "Save|Slot")
	void SaveGameToSlot();

	UFUNCTION(BlueprintCallable, Category = "Save|Slot")
	void LoadGameToSlot();

	UFUNCTION(BlueprintCallable, Category = "Save|Widget")
	void OpenSaveMenu();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Save|Widget")
	void OpenSaveMenuWidget();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save|Thumbnail")
	TObjectPtr<USceneCaptureComponent2D> SceneCaptureComponent2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Thumbnail")
	TObjectPtr<UTextureRenderTarget2D> ThumbnailRenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Widget")
	TObjectPtr<UUserWidget> SaveGameMenuWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Widget")
	TSubclassOf<UUserWidget> SaveGameMenuWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Widget")
	TObjectPtr<UUserWidget> SaveGameMessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save|Widget")
	TSubclassOf<UUserWidget> SaveGameMessageWidgetClass;

protected:
	UFUNCTION(BlueprintCallable, Category = "Save|Widget")
	void CloseSaveMenuWidget();

	UFUNCTION(BlueprintCallable, Category = "Save|Widget")
	void SaveCustom();

	UFUNCTION(BlueprintCallable, Category = "Save|Widget")
	void ShowSavingMessage();
};
