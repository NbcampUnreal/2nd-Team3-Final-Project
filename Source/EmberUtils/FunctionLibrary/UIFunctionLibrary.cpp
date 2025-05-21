#include "UIFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEmber//UI/HUD/EmberMainHUD.h"

FVector2D UUIFunctionLibrary::MousePos = FVector2D::ZeroVector;

bool UUIFunctionLibrary::RegisterLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag, UEmberLayerBase* Layer)
{
	if (OwningPlayer)
	{
		if (AEmberMainHUD* MainHUD = Cast<AEmberMainHUD>(OwningPlayer->GetHUD()))
		{
			MainHUD->RegisterLayer(LayerTag, Layer);
		}
	}

	return true;
}

UUserWidget* UUIFunctionLibrary::PushContentToLayer(APlayerController* OwningPlayer, const FGameplayTag LayerTag,
                                                    const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (OwningPlayer)
	{
		return Cast<AEmberMainHUD>(OwningPlayer->GetHUD())->PushContentToLayer(LayerTag, WidgetClass);
	}

	return nullptr;
}

void UUIFunctionLibrary::PopContentToLayer(APlayerController* OwningPlayer, const FGameplayTag LayerTag)
{
	if (OwningPlayer)
	{
		Cast<AEmberMainHUD>(OwningPlayer->GetHUD())->PopContentToLayer(LayerTag);
	}
}

void UUIFunctionLibrary::ClearToLayer(APlayerController* OwningPlayer, const FGameplayTag LayerTag)
{
	if (OwningPlayer)
	{
		Cast<AEmberMainHUD>(OwningPlayer->GetHUD())->ClearToLayer(LayerTag);
	}
}

void UUIFunctionLibrary::FocusGame(APlayerController* OwningPlayer)
{
	if (OwningPlayer)
	{
		OwningPlayer->GetMousePosition(MousePos.X, MousePos.Y);
		OwningPlayer->SetShowMouseCursor(false);
		OwningPlayer->SetIgnoreLookInput(false);
		OwningPlayer->SetInputMode(FInputModeGameOnly());
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
		UGameplayStatics::SetGamePaused(OwningPlayer->GetWorld(), false);

		if (AEmberMainHUD* MainHUD = Cast<AEmberMainHUD>(OwningPlayer->GetHUD()))
		{
			MainHUD->SetGameLeftMouseInputLock(false);
			MainHUD->SetGameMovementInputLock(false);
		}
	}
}

void UUIFunctionLibrary::FocusUI(APlayerController* OwningPlayer, UUserWidget* WidgetToFocus, bool ShowMouseCursor,
                                 bool bIgnoreMovement, bool bGameLeftMouseInputLock)
{
	if (!OwningPlayer || !WidgetToFocus)
	{
		EMBER_LOG(LogEmber, Error, TEXT("OwningPlayer or WidgetToFocus is null"));
		return;
	}

	// True 일때만 캐릭터 이동인풋 막기
	if (bIgnoreMovement)
	{
		Cast<AEmberMainHUD>(OwningPlayer->GetHUD())->SetGameMovementInputLock(true);
	}

	// True 일때만 Hud에게 좌클릭 인풋 막기
	if (bGameLeftMouseInputLock)
	{
		Cast<AEmberMainHUD>(OwningPlayer->GetHUD())->SetGameLeftMouseInputLock(true);
	}

	// True 일때만 마우스커서 보이게 하기 ( 마우스 커서 보이게 하면 카메라 회전도 막음 )
	if (ShowMouseCursor)
	{
		OwningPlayer->SetMouseLocation(MousePos.X, MousePos.Y);
		OwningPlayer->SetShowMouseCursor(true);
		OwningPlayer->SetIgnoreLookInput(true);
	}

	// 위젯에 포커스 주기
	OwningPlayer->bEnableMouseOverEvents = true;
	OwningPlayer->bEnableClickEvents = true;
	WidgetToFocus->SetUserFocus(OwningPlayer);
	WidgetToFocus->SetKeyboardFocus();

	FInputModeGameAndUI Mode;
	Mode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	OwningPlayer->SetInputMode(Mode);
}

bool UUIFunctionLibrary::GetIsAbilityInputLock(APlayerController* OwningPlayer)
{
	return Cast<AEmberMainHUD>(OwningPlayer->GetHUD())->GetGameLeftMouseInputLock();
}

bool UUIFunctionLibrary::GetIsGameMovementInputLock(APlayerController* OwningPlayer)
{
	return Cast<AEmberMainHUD>(OwningPlayer->GetHUD())->GetGameMovementInputLock();
}
