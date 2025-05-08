#include "EmberUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"

void UEmberUserWidget::SetAbilitySystemComponent(class AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
	else
	{
		check(true);
	}
}

class UAbilitySystemComponent* UEmberUserWidget::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}