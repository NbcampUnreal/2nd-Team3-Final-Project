// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Layer/EmberLayerBase.h"
#include "LoopInfoOverlayWidget.generated.h"

class UMediaTexture;
class UImage;
class UMediaSource;
class UMediaPlayer;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoopMessagesFinished);
/**
 * 
 */
USTRUCT(BlueprintType)
struct FLoopMessageData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMediaSource> MediaSource = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInterface> Material = nullptr;
};

UCLASS()
class PROJECTEMBER_API ULoopInfoOverlayWidget : public UEmberLayerBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StartLoopMessage(const TArray<FLoopMessageData>& InMessages);
	UFUNCTION(BlueprintCallable)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnLoopMessagesFinished OnLoopMessagesFinished;
	
protected:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY()
	class UTextBlock* CurrentLineText;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* MessageBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameLoop")
	USoundBase* TypingSound;
	
private:
	void StartTypingCurrentMessage();
	void PlayMedia(const TObjectPtr<UMediaSource>& MediaSource);
	void StopMedia();
	void ApplyMediaMaterial(const TObjectPtr<UMaterialInterface>& Material);
	void AddNewLineTextBlock();
	void TypeNextChar();
	void ProceedToNextMessage();

	void OnAnyKeyPressed();

	UPROPERTY(EditAnywhere, Category = "GameLoop")
	TObjectPtr<UMediaPlayer> MediaPlayer;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	TObjectPtr<UImage> MediaImage;

	UPROPERTY(EditAnywhere, Category="Media")
	TObjectPtr<UMediaTexture> MediaTexture;
	
	TArray<FLoopMessageData> Messages;
	TArray<FString> Lines;
	int32 CurrentMessageIndex = 0;
	int32 CurrentLineIndex = 0;
	int32 CurrentCharIndex = 0;
	
	FTimerHandle TimerHandle;
	float TypingInterval = 0.05;
	
	bool bIsTyping = false;
	bool bWaitingForInput = false;
};


