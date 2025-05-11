#include "EmberLayerBase.h"


UUserWidget* UEmberLayerBase::PushWidget(const TSubclassOf<UUserWidget>& WidgetClass)
{
	if (UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayer(), WidgetClass))
	{
		CollapseTopWidget();
		Border->ClearChildren();
	
		Stack.Add(Widget);
		Border->AddChild(Widget);
	
		ShowTopWidget();
		return Widget;
	}
	return nullptr;
}

void UEmberLayerBase::PopWidget()
{
	if (UUserWidget* TopWidget = GetTopWidget())
	{
		TopWidget->RemoveFromParent();
		Stack.Remove(TopWidget);
		Border->ClearChildren();

		if (UUserWidget* NextWidget = GetTopWidget())
		{
			NextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Border->AddChild(NextWidget);
		}
	}
}

void UEmberLayerBase::ClearStack()
{
	for (UUserWidget* Widget : Stack)
	{
		if (IsValid(Widget))
		{
			Widget->RemoveFromParent();
		}
	}
	
	Stack.Empty();
	
	if (Border)
	{
		Border->ClearChildren();
	}
}

UUserWidget* UEmberLayerBase::GetTopWidget()
{
	if (Stack.IsEmpty())
	{
		return nullptr;
	}

	return Stack.Last().Get();
}

void UEmberLayerBase::CollapseTopWidget()
{
	if (UUserWidget* TopWidget = GetTopWidget())
	{
		TopWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UEmberLayerBase::ShowTopWidget()
{
	if (UUserWidget* TopWidget = GetTopWidget())
	{
		TopWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

FText UEmberLayerBase::GetStackListNames()
{
	TArray<FString> Names;
	Names.Reserve(Stack.Num());
	
	for (int32 i = Stack.Num() - 1; i >= 0; --i)
	{
		if (const UUserWidget* W = Stack[i].Get())
		{
			Names.Add(W->GetName());
		}
	}

	const FString Joined = FString::Join(Names,TEXT(" "));
	return FText::FromString(Joined);
}
