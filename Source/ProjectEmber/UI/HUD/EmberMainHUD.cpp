#include "EmberMainHUD.h"
#include "AI_NPC/Widget/PlayerQuestWidget.h"
#include "Quest/QuestSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "EmberLog/EmberLog.h"
#include "UI/BaseWidget/GameMenuWidget.h"
#include "UI/Debug/LayerDebugger.h"
#include "UI/Layer/EmberLayerBase.h"

void AEmberMainHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayerController(), PrimaryLayoutClass))
	{
		Widget->AddToViewport();
		PushInitialWidget();

#if !UE_BUILD_SHIPPING
		/*if (UWidget* DebugLayer = Widget->GetWidgetFromName(TEXT("LayerDebugger")))
		{
			PrimaryDebugLayer = Cast<ULayerDebugger>(DebugLayer);
		}*/
#endif
	}
	else
	{
		EMBER_LOG(LogTemp, Error, TEXT("Failed to create primary layout widget."));
	}

}

bool AEmberMainHUD::RegisterLayer(const FGameplayTag& LayerTag, UUserWidget* Layer)
{
	// UEmberLayerBase로 캐스트해야만 유효하므로 체크
	if (!Layer)
	{
		EMBER_LOG(LogTemp, Warning, TEXT("RegisterLayer: Layer pointer is null"));
		return false;
	}

	UEmberLayerBase* LayerObj = Cast<UEmberLayerBase>(Layer);
	if (!IsValid(LayerObj))
	{
		EMBER_LOG(LogTemp, Warning, TEXT("RegisterLayer: Passed widget is not a UEmberLayerBase"));
		return false;
	}

	if (!EmberLayers.Contains(LayerTag))
	{
		EmberLayers.Add(LayerTag, LayerObj);

#if !UE_BUILD_SHIPPING
		/*if (PrimaryDebugLayer)
		{
			PrimaryDebugLayer->SetChangedLayer();
		}*/
#endif

		return true;
	}

	EMBER_LOG(LogTemp, Warning, TEXT("Layer already registered: %s"), *LayerTag.ToString());
	return false;
}

UUserWidget* AEmberMainHUD::PushContentToLayer(const FGameplayTag& LayerTag, const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (UEmberLayerBase* FoundLayer = *EmberLayers.Find(LayerTag))
	{
		UUserWidget* PushWidget = FoundLayer->PushWidget(WidgetClass);

#if !UE_BUILD_SHIPPING
		/*if (PrimaryDebugLayer)
		{
			PrimaryDebugLayer->SetChangedLayer();
		}*/
#endif

		return PushWidget;
	}
	return nullptr;
}

void AEmberMainHUD::PopContentToLayer(const FGameplayTag& LayerTag)
{
	if (UEmberLayerBase* FoundLayer = *EmberLayers.Find(LayerTag))
	{
		FoundLayer->PopWidget();

#if !UE_BUILD_SHIPPING
		/*if (PrimaryDebugLayer)
		{
			PrimaryDebugLayer->SetChangedLayer();
		}*/
#endif
	}
}

void AEmberMainHUD::ClearToLayer(const FGameplayTag& LayerTag)
{
	if (UEmberLayerBase* FoundLayer = *EmberLayers.Find(LayerTag))
	{
		FoundLayer->ClearStack();

#if !UE_BUILD_SHIPPING
		/*if (PrimaryDebugLayer)
		{
			PrimaryDebugLayer->SetChangedLayer();
		}*/
#endif
	}
}

bool AEmberMainHUD::GetGameLeftMouseInputLock() const
{
	return bIsGameLeftMouseInputLock;
}

void AEmberMainHUD::SetGameLeftMouseInputLock(bool bLock)
{
	bIsGameLeftMouseInputLock = bLock;
}

bool AEmberMainHUD::GetGameMovementInputLock() const
{
	return bIsGameMovementInputLock;
}

void AEmberMainHUD::SetGameMovementInputLock(bool bLock)
{
	bIsGameMovementInputLock = bLock;
}

void AEmberMainHUD::PushInitialWidget()
{
	for (auto& Pair : InitWidgetClasses)
	{
		PushContentToLayer(Pair.Key, Pair.Value);

#if !UE_BUILD_SHIPPING
		/*if (PrimaryDebugLayer)
		{
			PrimaryDebugLayer->SetChangedLayer();
		}*/
#endif
	}
}

UEmberLayerBase* AEmberMainHUD::GetLayer(const FGameplayTag& LayerTag) const
{
	if (UEmberLayerBase* FoundLayer = *EmberLayers.Find(LayerTag))
	{
		return FoundLayer;
	}
	return nullptr;
}

UPlayerQuestWidget* AEmberMainHUD::GetQuestLogWidget() const
{
	return PlayerQuestWidgetInstance;
}

