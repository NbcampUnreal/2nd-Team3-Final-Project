#include "UIFunctionLibrary.h"
#include "EmberLog/EmberLog.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Interface/EmberHUDInterface.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"

FVector2D UUIFunctionLibrary::MousePos = FVector2D::ZeroVector;

bool UUIFunctionLibrary::RegisterLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag, UUserWidget* Layer)
{
    if (!OwningPlayer)
    {
        return false;
    }
    
    if (IEmberHUDInterface* HUDInterface = Cast<IEmberHUDInterface>(OwningPlayer->GetHUD()))
    {
        HUDInterface->RegisterLayer(LayerTag, Layer);
        return true;
    }

    return false;
}

UUserWidget* UUIFunctionLibrary::PushContentToLayer(APlayerController* OwningPlayer, const FGameplayTag LayerTag,
                                                    const TSubclassOf<UUserWidget>& WidgetClass)
{
    if (!OwningPlayer)
    {
        return nullptr;
    }

    if (IEmberHUDInterface* HUDInterface = Cast<IEmberHUDInterface>(OwningPlayer->GetHUD()))
    {
        return HUDInterface->PushContentToLayer(LayerTag, WidgetClass);
    }

    return nullptr;
}

void UUIFunctionLibrary::PopContentToLayer(APlayerController* OwningPlayer, const FGameplayTag LayerTag)
{
    if (!OwningPlayer)
    {
        return;
    }

    if (IEmberHUDInterface* HUDInterface = Cast<IEmberHUDInterface>(OwningPlayer->GetHUD()))
    {
        HUDInterface->PopContentToLayer(LayerTag);
    }
}

void UUIFunctionLibrary::ClearToLayer(APlayerController* OwningPlayer, const FGameplayTag LayerTag)
{
    if (!OwningPlayer)
    {
        return;
    }

    if (IEmberHUDInterface* HUDInterface = Cast<IEmberHUDInterface>(OwningPlayer->GetHUD()))
    {
        HUDInterface->ClearToLayer(LayerTag);
    }
}

void UUIFunctionLibrary::FocusGame(APlayerController* OwningPlayer)
{
    if (!OwningPlayer)
    {
        return;
    }

    OwningPlayer->GetMousePosition(MousePos.X, MousePos.Y);
    OwningPlayer->SetShowMouseCursor(false);
    OwningPlayer->SetIgnoreLookInput(false);
    OwningPlayer->SetInputMode(FInputModeGameOnly());
    UWidgetBlueprintLibrary::SetFocusToGameViewport();
    UGameplayStatics::SetGamePaused(OwningPlayer->GetWorld(), false);

    // 인터페이스로 좌클릭/이동잠금 호출
    if (IEmberHUDInterface* HUDInterface = Cast<IEmberHUDInterface>(OwningPlayer->GetHUD()))
    {
        HUDInterface->SetGameLeftMouseInputLock(false);
        HUDInterface->SetGameMovementInputLock(false);
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

    if (IEmberHUDInterface* HUDInterface = Cast<IEmberHUDInterface>(OwningPlayer->GetHUD()))
    {
        if (bIgnoreMovement)
        {
            HUDInterface->SetGameMovementInputLock(true);
        }
        if (bGameLeftMouseInputLock)
        {
            HUDInterface->SetGameLeftMouseInputLock(true);
        }
    }

    if (ShowMouseCursor)
    {
        OwningPlayer->SetMouseLocation(MousePos.X, MousePos.Y);
        OwningPlayer->SetShowMouseCursor(true);
        OwningPlayer->SetIgnoreLookInput(true);
    }

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
    if (!OwningPlayer)
    {
        return false;
    }
    if (IEmberHUDInterface* HUDInterface = Cast<IEmberHUDInterface>(OwningPlayer->GetHUD()))
    {
        return HUDInterface->GetGameLeftMouseInputLock();
    }
    return false;
}

bool UUIFunctionLibrary::GetIsGameMovementInputLock(APlayerController* OwningPlayer)
{
    if (!OwningPlayer)
    {
        return false;
    }
    if (IEmberHUDInterface* HUDInterface = Cast<IEmberHUDInterface>(OwningPlayer->GetHUD()))
    {
        return HUDInterface->GetGameMovementInputLock();
    }
    return false;
}
