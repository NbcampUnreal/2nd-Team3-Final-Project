#include "EmberWidgetComponent.h"
#include "EmberUserWidget.h"

void UEmberWidgetComponent::UpdateAbilitySystemComponent() const
{
	if (UEmberUserWidget* EmberUserWidget = Cast<UEmberUserWidget>(GetWidget()))
	{
		EmberUserWidget->SetAbilitySystemComponent(GetOwner());
	}
}

void UEmberWidgetComponent::InitWidget()
{
	Super::InitWidget();

	if (UEmberUserWidget* EmberUserWidget = Cast<UEmberUserWidget>(GetWidget()))
	{
		EmberUserWidget->SetAbilitySystemComponent(GetOwner());
	}
}
