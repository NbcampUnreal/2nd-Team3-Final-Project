#include "EmberWidgetComponent.h"
#include "EmberUserWidget.h"

void UEmberWidgetComponent::UpdateAbilitySystemComponent(AActor* InOwner) const
{
	if (UEmberUserWidget* EmberUserWidget = Cast<UEmberUserWidget>(GetWidget()))
	{
		EmberUserWidget->SetAbilitySystemComponent(InOwner);
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
