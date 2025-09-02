// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Layer/EmberLayerBase.h"
#include "LoopInfoOverlayWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoopMessagesFinished);
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API ULoopInfoOverlayWidget : public UEmberLayerBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StartLoopMessage(const TArray<FString>& InMessages);
	UFUNCTION(BlueprintCallable)
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnLoopMessagesFinished OnLoopMessagesFinished;
	
protected:
	UPROPERTY()
	class UTextBlock* CurrentLineText;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* MessageBox;
	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameLoop")
	USoundBase* TypingSound;
	
private:
	void StartTypingCurrentMessage();
	void AddNewLineTextBlock();
	void TypeNextChar();
	void ProceedToNextMessage();

	void OnAnyKeyPressed();

	TArray<FString> Messages;
	TArray<FString> Lines;
	int32 CurrentMessageIndex = 0;
	int32 CurrentLineIndex = 0;
	int32 CurrentCharIndex = 0;
	
	FTimerHandle TimerHandle;
	float TypingInterval = 0.05;
	
	bool bIsTyping = false;
	bool bWaitingForInput = false;
};
