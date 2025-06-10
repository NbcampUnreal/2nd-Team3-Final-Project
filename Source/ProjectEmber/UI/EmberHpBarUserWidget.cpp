#include "EmberHpBarUserWidget.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UEmberHpBarUserWidget::SetAbilitySystemComponent(class AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetMaxManaAttribute()).AddUObject(this, &ThisClass::OnMaxManaChanged);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetShieldAttribute()).AddUObject(this, &ThisClass::OnShieldChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetMaxShieldAttribute()).AddUObject(this, &ThisClass::OnMaxShieldChanged);
		//AbilitySystemComponent->RegisterGameplayTagEvent(EmberGameplayTag::Character_State_Invincible, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnInvincibleTagChanged);
		
		//PbHpBar->SetFillColorAndOpacity(HealthColor);

		if (const UEmberCharacterAttributeSet* CurrentAttributeSet = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
		{
			CurrentAttributeHealth = CurrentAttributeSet->GetHealth();
			CurrentAttributeMaxHealth = CurrentAttributeSet->GetMaxHealth();
			
			CurrentMana = CurrentAttributeSet->GetMana();
			CurrentMaxMana = CurrentAttributeSet->GetMaxMana();

			CurrentShield = CurrentAttributeSet->GetShield();
			CurrentMaxShield = CurrentAttributeSet->GetMaxShield();
			
			//UpdateHpBar();
		}
	}
}



void UEmberHpBarUserWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentAttributeHealth = ChangeData.NewValue;
	
	//UpdateHpBar();
}

void UEmberHpBarUserWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentAttributeMaxHealth = ChangeData.NewValue;
	
	//UpdateHpBar();
}

void UEmberHpBarUserWidget::OnInvincibleTagChanged(const FGameplayTag GameplayTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		//PbHpBar->SetFillColorAndOpacity(InvincibleColor);
		PbHpBar->SetPercent(1.0f);
	}
	else
	{
		//PbHpBar->SetFillColorAndOpacity(HealthColor);
		UpdateHpBar();
	}
}

void UEmberHpBarUserWidget::UpdateHpBar() const
{
	/*if (PbHpBar)
	{
		PbHpBar->SetPercent(CurrentHealth / CurrentMaxHealth);
	}*/

	/*if (TxtHpStat)
	{
		TxtHpStat->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentHealth, CurrentMaxHealth)));
	}*/
}

void UEmberHpBarUserWidget::OnManaChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMana = ChangeData.NewValue;
}

void UEmberHpBarUserWidget::OnMaxManaChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxMana = ChangeData.NewValue;
}

void UEmberHpBarUserWidget::OnShieldChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentShield = ChangeData.NewValue;
}

void UEmberHpBarUserWidget::OnMaxShieldChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxShield = ChangeData.NewValue;
}