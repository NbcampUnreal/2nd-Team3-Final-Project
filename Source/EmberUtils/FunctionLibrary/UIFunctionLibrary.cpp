#include "UIFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectEmber//UI/HUD/EmberMainHUD.h"

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
		OwningPlayer->bShowMouseCursor = false;
		OwningPlayer->SetInputMode(FInputModeGameOnly());
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
		UGameplayStatics::SetGamePaused(OwningPlayer->GetWorld(), false);
	}
}
