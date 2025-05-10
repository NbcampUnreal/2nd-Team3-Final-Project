#include "UIManager.h"
#include "UI/BaseWidget/EmberBaseWidget.h"

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	RegisterLayer(ELayer::Game,     WidgetClasses.FindChecked(ELayer::Game));
	RegisterLayer(ELayer::GameMenu, WidgetClasses.FindChecked(ELayer::GameMenu));
	RegisterLayer(ELayer::Menu,     WidgetClasses.FindChecked(ELayer::Menu));
	RegisterLayer(ELayer::Modal,    WidgetClasses.FindChecked(ELayer::Modal));
}

void UUIManager::RegisterLayer(const ELayer Layer, const TSubclassOf<UEmberBaseWidget>& WidgetClass)
{
	WidgetClasses.Add(Layer, WidgetClass);
}

void UUIManager::Open(const ELayer Layer)
{
	if (UEmberBaseWidget* Widget = GetOrCreate(Layer))
	{
		Widget->Show();
	}
}

void UUIManager::Close(const ELayer Layer)
{
	if (UEmberBaseWidget* Widget = GetOrCreate(Layer))
	{
		Widget->Hide();
	}
}

void UUIManager::Toggle(const ELayer Layer)
{
	if (UEmberBaseWidget* Widget = GetOrCreate(Layer))
	{
		Widget->Toggle();
	}
}

UEmberBaseWidget* UUIManager::GetOrCreate(ELayer Layer)
{
	if (Widgets.Contains(Layer))
	{
		return Widgets[Layer];
	}
	
	if (const TSubclassOf<UEmberBaseWidget> WidgetClass = WidgetClasses.FindRef(Layer))
	{
		if (UEmberBaseWidget* Widget = CreateWidget<UEmberBaseWidget>(GetWorld(), WidgetClass))
		{
			Widget->AddToViewport(static_cast<int32>(Layer) * 100);
			Widgets.Add(Layer, Widget);
			return Widget;
		}
	}
	return nullptr;
}

