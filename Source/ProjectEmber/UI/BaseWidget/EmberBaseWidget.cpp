#include "EmberBaseWidget.h"

void UEmberBaseWidget::Show()
{
	PlayAnimation(OpenAnim);
	OnOpened();
}

void UEmberBaseWidget::Hide()
{
	PlayAnimation(CloseAnim);
	OnClosed();
}

void UEmberBaseWidget::Toggle()
{
	bIsVisible ? Hide() : Show();
}
