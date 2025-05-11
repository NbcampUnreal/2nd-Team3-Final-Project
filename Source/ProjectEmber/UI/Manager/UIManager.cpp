#include "UIManager.h"
#include "UIManagerHelper.h"

void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	//WidgetClassesHelper = NewObject<UUIManagerHelper>(GetTransientPackage(), WidgetClassesHelperClass);
}

void UUIManager::Open(const ELayer Layer)
{
	if (UEmberBaseWidget* Widget = GetOrCreate(Layer))
	{
		//Widget->Show();
	}
}

void UUIManager::Close(const ELayer Layer)
{
	if (UEmberBaseWidget* Widget = GetOrCreate(Layer))
	{
		//Widget->Hide();
	}
}

void UUIManager::Toggle(const ELayer Layer)
{
	if (UEmberBaseWidget* Widget = GetOrCreate(Layer))
	{
		//Widget->Toggle();
	}
}

UEmberBaseWidget* UUIManager::GetOrCreate(ELayer Layer)
{
	if (Widgets.Contains(Layer))
	{
		return Widgets[Layer];
	}
	
	/*
	if (const TSubclassOf<UEmberBaseWidget> WidgetClass = WidgetClassesHelper->WidgetClasses.FindRef(Layer))
	{
		if (UEmberBaseWidget* Widget = CreateWidget<UEmberBaseWidget>(GetWorld(), WidgetClass))
		{
			Widget->AddToViewport(static_cast<int32>(Layer) * 100);
			Widgets.Add(Layer, Widget);
			return Widget;
		}
	}*/
	return nullptr;
}

