#include "EmberMainHUD.h"
#include "AI_NPC/PlayerQuestWidget.h"
#include "AI_NPC/QuestReceiverComponent.h"
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
			if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(GetOwningPlayerController()->GetPawn()))
			{
				if (EmberCharacter->QuestReceiverComponent)
				{
					EmberCharacter->QuestReceiverComponent->SetQuestLogWidget(PlayerQuestWidgetInstance);
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
void AEmberMainHUD::UpdateQuestLogWidget(const FQuestDataRow& QuestRow)
{
	if (PlayerQuestWidgetInstance)
	{
		bool bIsComplete = false;

		// 현재 캐릭터가 해당 퀘스트를 완료했는지 판단
		if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		{
			if (UQuestReceiverComponent* QuestReceiver = EmberCharacter->FindComponentByClass<UQuestReceiverComponent>())
			{
				bIsComplete = QuestReceiver->IsQuestComplete(QuestRow.QuestID);
			}
		}

		PlayerQuestWidgetInstance->SetQuestInfoFromDataRow(QuestRow, bIsComplete);
	}
}