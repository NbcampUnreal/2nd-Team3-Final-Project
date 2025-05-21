#include "EmberUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"

void UEmberUserWidget::SetAbilitySystemComponent(class AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
}

class UAbilitySystemComponent* UEmberUserWidget::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}