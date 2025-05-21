#include "EmberMainHUD.h"

#include "InputActionValue.h"
#include "EmberLog/EmberLog.h"
#include "UI/Debug/LayerDebugger.h"
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
			PrimaryDebugLayer = Cast<ULayerDebugger>(DebugLayer);
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
			//PrimaryDebugLayer->SetChangedLayer();
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
		//PrimaryDebugLayer->SetChangedLayer();
	}
}

void AEmberMainHUD::PopContentToLayer(const FGameplayTag LayerTag)
{
	if (UEmberLayerBase* Layer = *EmberLayers.Find(LayerTag))
	{
		Layer->PopWidget();
		//PrimaryDebugLayer->SetChangedLayer();
	}
}

UUserWidget* AEmberMainHUD::PushContentToLayer(const FGameplayTag LayerTag, const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (UEmberLayerBase* Layer = *EmberLayers.Find(LayerTag))
	{
		UUserWidget* PushWidget = Layer->PushWidget(WidgetClass);
		//PrimaryDebugLayer->SetChangedLayer();
		return PushWidget;
	}

	return nullptr;
}

void AEmberMainHUD::ClearToLayer(FGameplayTag LayerTag)
{
	if (UEmberLayerBase* Layer = *EmberLayers.Find(LayerTag))
	{
		Layer->ClearStack();
		//PrimaryDebugLayer->SetChangedLayer();
	}
}

void AEmberMainHUD::SetGameLeftMouseInputLock(bool bLock)
{
	bIsGameLeftMouseInputLock = bLock;
}

bool AEmberMainHUD::GetGameLeftMouseInputLock()
{
	return bIsGameLeftMouseInputLock;
}

void AEmberMainHUD::SetGameMovementInputLock(bool bLock)
{
	bIsGameMovementInputLock = bLock;
}

bool AEmberMainHUD::GetGameMovementInputLock()
{
	return bIsGameMovementInputLock;
}

void AEmberMainHUD::Input_ToggleLayer(const FInputActionValue& ActionValue, FGameplayTag LayerTag,
	FName LayerWidgetName)
{
	if (IsValid(EmberLayers[LayerTag]))
	{
		EmberLayers[LayerTag]->Input_ToggleLayer(LayerWidgetName);
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