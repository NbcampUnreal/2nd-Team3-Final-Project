// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Loop/LoopInfoOverlayWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Loop/GameLoopManagerSubsystem.h"

void ULoopInfoOverlayWidget::StartLoopMessage(const TArray<FString>& InMessages)
{
	Messages.Empty();

	Messages.Add(FString::Printf(TEXT("플레이어의 의식이 끊어졌습니다.\n다시 재가동 합니다.")));

	Messages.Append(InMessages);
	
	if (UGameLoopManagerSubsystem* LoopManager = GetGameInstance()->GetSubsystem<UGameLoopManagerSubsystem>())
	{
		int32 CurrentLoop = LoopManager->GetCurrentLoopID();
		Messages.Add(FString::Printf(TEXT("%i번째 루프"), CurrentLoop));
	}
	
	CurrentMessageIndex = 0;
	CurrentCharIndex = 0;
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	SetIsFocusable(true);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	PC->bShowMouseCursor = false;
	PC->SetInputMode(InputMode);

	// AddToViewport 된 다음에 포커스 잡을거임
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		SetKeyboardFocus();
	});
	
	StartTypingCurrentMessage();
}

void ULoopInfoOverlayWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

FReply ULoopInfoOverlayWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.IsRepeat())
	{
		return FReply::Handled();
	}
	
	OnAnyKeyPressed();
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void ULoopInfoOverlayWidget::StartTypingCurrentMessage()
{
	MessageBox->ClearChildren();
	CurrentCharIndex = 0;
	CurrentLineIndex = 0;
	Lines.Empty();

	Messages[CurrentMessageIndex].ParseIntoArrayLines(Lines);

	AddNewLineTextBlock();
	
	bIsTyping = true;
	bWaitingForInput = false;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ULoopInfoOverlayWidget::TypeNextChar, TypingInterval, true);
}

void ULoopInfoOverlayWidget::AddNewLineTextBlock()
{
	UTextBlock* NewLine = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
	NewLine->SetText(FText::FromString(TEXT("")));

	if (UVerticalBoxSlot* VSlot = MessageBox->AddChildToVerticalBox(NewLine))
	{
		VSlot->SetHorizontalAlignment(HAlign_Center);
	}

	CurrentLineText = NewLine;
	CurrentCharIndex = 0;
}

void ULoopInfoOverlayWidget::TypeNextChar()
{
	const FString& FullLine = Lines[CurrentLineIndex];
	CurrentCharIndex++;

	// 텍스트 박스에 한 글자 추가 후 업데이트
	if (CurrentLineText)
	{
		CurrentLineText->SetText(FText::FromString(FullLine.Left(CurrentCharIndex)));
	}

	// 텍스트 줄 입력 끝
	
	if (CurrentCharIndex >= FullLine.Len())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		bIsTyping = false;
		bWaitingForInput  = true;
	}
	else if (!FChar::IsWhitespace(FullLine[CurrentCharIndex]) && TypingSound)
	{
		UGameplayStatics::PlaySound2D(this, TypingSound);
	}
}

void ULoopInfoOverlayWidget::ProceedToNextMessage()
{
	CurrentMessageIndex++;
	if (CurrentMessageIndex < Messages.Num())
	{
		StartTypingCurrentMessage();
	}
	else
	{
		// 메시지 끝. 위젯 숨기기 혹은 종료 콜백
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}
		
		SetVisibility(ESlateVisibility::Collapsed);

		OnLoopMessagesFinished.Broadcast();
	}
}

void ULoopInfoOverlayWidget::OnAnyKeyPressed()
{
	if (bIsTyping)
	{
		// 스킵하고 전체 문장 출력
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		CurrentLineText->SetText(FText::FromString(Messages[CurrentMessageIndex]));
		bIsTyping = false;
		bWaitingForInput = true;
		return;
	}

	if (bWaitingForInput)
	{
		// 현재 메시지의 남은 부분 계속 출력
		CurrentLineIndex++;

		if (CurrentLineIndex < Lines.Num())
		{
			AddNewLineTextBlock();
			bWaitingForInput = false;
			bIsTyping = true;

			GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&ULoopInfoOverlayWidget::TypeNextChar,
			TypingInterval,
			true
			);
		}
		else
		{
			ProceedToNextMessage();
		}
		
	}
}
