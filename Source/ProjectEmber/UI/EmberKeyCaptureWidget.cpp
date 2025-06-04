#include "EmberKeyCaptureWidget.h"
#include "Framework/Application/SlateApplication.h"

UEmberKeyCaptureWidget::UEmberKeyCaptureWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SetIsFocusable(true);
}

void UEmberKeyCaptureWidget::NativeConstruct()
{
    Super::NativeConstruct();
    SetKeyboardFocus();
}

FReply UEmberKeyCaptureWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey PressedKey = InKeyEvent.GetKey();

    if (PressedKey.IsValid())
    {
        OnKeyCaptured.Broadcast(PressedKey);

        RemoveFromParent();

        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}