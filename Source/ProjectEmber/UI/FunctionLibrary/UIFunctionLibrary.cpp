#include "UIFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "UI/HUD/EmberMainHUD.h"

bool UUIFunctionLibrary::RegisterLayer(APlayerController* OwningPlayer, FGameplayTag LayerTag, UEmberLayerBase* Layer)
{
	if (IsValid(OwningPlayer))
	{
		if (AEmberMainHUD* MainHUD = Cast<AEmberMainHUD>(OwningPlayer->GetHUD()))
		{
			MainHUD->RegisterLayer(LayerTag, Layer);
		}
	}
	
	return true;
}
