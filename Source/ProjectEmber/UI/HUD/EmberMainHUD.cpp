#include "EmberMainHUD.h"
#include "EmberLog/EmberLog.h"
#include "UI/Layer/EmberLayerBase.h"

void AEmberMainHUD::BeginPlay()
{
	Super::BeginPlay();
	
	if (UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PrimaryLayoutClass))
	{
		Widget->AddToViewport();
		PushInitialWidget();

//#if !UE_BUILD_SHIPPING
		if (UWidget* DebugLayer = Widget->GetWidgetFromName(TEXT("LayerDebugger")))
		{
			PrimaryDebugLayer = Cast<UUserWidget>(DebugLayer);
		}
//#endif
	}
	else
	{
		EMBER_LOG(LogTemp, Error, TEXT("Failed to create primary layout widget."));
	}
}

bool AEmberMainHUD::RegisterLayer(const FGameplayTag LayerTag, UEmberLayerBase* Layer)
{
	if (IsValid(Layer))
	{
		if (!EmberLayers.Contains(LayerTag))
		{
			EmberLayers.Add(LayerTag, Layer);
			return true;
		}
	}

	EMBER_LOG(LogTemp, Warning, TEXT("Layer already registered: %s"), *LayerTag.ToString());
	return false;
}

void AEmberMainHUD::PushInitialWidget()
{
	for (const auto WidgetClass : InitWidgetClasses)
	{
		PushContentToLayer(WidgetClass.Key, WidgetClass.Value);
	}
}

void AEmberMainHUD::PopContentToLayer(const FGameplayTag LayerTag)
{
	if (UEmberLayerBase* Layer = *EmberLayers.Find(LayerTag))
	{
		Layer->PopWidget();
	}
}

UUserWidget* AEmberMainHUD::PushContentToLayer(const FGameplayTag LayerTag, const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (UEmberLayerBase* Layer = *EmberLayers.Find(LayerTag))
	{
		return Layer->PushWidget(WidgetClass);
	}

	return nullptr;
}

void AEmberMainHUD::ClearToLayer(FGameplayTag LayerTag)
{
	if (UEmberLayerBase* Layer = *EmberLayers.Find(LayerTag))
	{
		Layer->ClearStack();
	}
}

//#if !UE_BUILD_SHIPPING
void AEmberMainHUD::ToggleDebugLayer()
{
	if (PrimaryDebugLayer)
	{
		bDebugLayerVisible = !bDebugLayerVisible;
		PrimaryDebugLayer->SetVisibility(bDebugLayerVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
//#endif