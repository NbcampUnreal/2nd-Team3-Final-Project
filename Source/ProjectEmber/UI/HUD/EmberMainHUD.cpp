#include "EmberMainHUD.h"
#include "AI_NPC/PlayerQuestWidget.h"
#include "Quest/QuestSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Character/EmberCharacter.h"
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
	if (PlayerQuestWidgetClass)
	{
		PlayerQuestWidgetInstance = CreateWidget<UPlayerQuestWidget>(GetOwningPlayerController(), PlayerQuestWidgetClass);
		if (PlayerQuestWidgetInstance)
		{
			if (UQuestSubsystem* QuestSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>())
			{
				FName CurrentQuestID;
				if (QuestSubsystem->GetLastActiveQuestID(CurrentQuestID))
				{
					if (UQuestDataAsset* QuestAsset = QuestSubsystem->GetAllLoadedQuests().FindRef(CurrentQuestID))
					{
						bool bIsComplete = QuestSubsystem->IsQuestCompleted(CurrentQuestID);
						bool bIsAccepted = QuestSubsystem->IsQuestAccepted(CurrentQuestID); // 수락 여부 추가
						PlayerQuestWidgetInstance->SetQuestInfoFromDataAsset(QuestAsset, bIsComplete, bIsAccepted);
					}
				}
			}
		}
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

UEmberLayerBase* AEmberMainHUD::GetLayer(FGameplayTag LayerTag) const
{
	return *EmberLayers.Find(LayerTag);
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
UPlayerQuestWidget* AEmberMainHUD::GetQuestLogWidget() const
{
	return PlayerQuestWidgetInstance;
}
//#endif
void AEmberMainHUD::UpdateQuestLogWidget(const UQuestDataAsset* QuestAsset)
{
	if (!PlayerQuestWidgetInstance || !QuestAsset) return;

	bool bIsComplete = false;
	bool bIsAccepted = false;

	if (UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		bIsComplete = QuestSubsystem->IsQuestCompleted(QuestAsset->QuestID);
		bIsAccepted = QuestSubsystem->IsQuestAccepted(QuestAsset->QuestID); // 수락 여부 추가
	}

	PlayerQuestWidgetInstance->SetQuestInfoFromDataAsset(QuestAsset, bIsComplete, bIsAccepted);
}