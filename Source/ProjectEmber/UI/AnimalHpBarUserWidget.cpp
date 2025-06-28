#include "AnimalHpBarUserWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UAnimalHpBarUserWidget::SetAbilitySystemComponent(class AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
		
		if (const UEmberCharacterAttributeSet* CurrentAttributeSet = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
		{
			CurrentAttributeHealth = CurrentAttributeSet->GetHealth();
			CurrentAttributeMaxHealth = CurrentAttributeSet->GetMaxHealth();
			
			UpdateHpBar();
		}
	}
}

void UAnimalHpBarUserWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentAttributeHealth = ChangeData.NewValue;
	
	UpdateHpBar();
}

void UAnimalHpBarUserWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentAttributeMaxHealth = ChangeData.NewValue;
	
	UpdateHpBar();
}

void UAnimalHpBarUserWidget::UpdateHpBar() const
{
	if (PbHpBar)
	{
		PbHpBar->SetPercent(CurrentAttributeHealth / CurrentAttributeMaxHealth);
	}
	if (TxtHpStat)
	{
		TxtHpStat->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentAttributeHealth, CurrentAttributeMaxHealth)));
	}
}