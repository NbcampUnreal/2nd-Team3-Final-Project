#include "EmberCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayTag/EmberGameplayTag.h"
#include "EmberLog/EmberLog.h"

UEmberCharacterAttributeSet::UEmberCharacterAttributeSet()
{
	InitAttackRate(30.0f);
	InitMaxAttackRate(100.0f);

	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	
	InitDamage(0.0f);

	InitMana(100.0f);
	InitMaxMana(100.0f);

	InitShield(100.0f);
	InitMaxShield(100.0f);
	
}

void UEmberCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}
}

void UEmberCharacterAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue,
	float NewValue)
{
	
}

bool UEmberCharacterAttributeSet::PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	
	// if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	// {
	// 	if (Data.EvaluatedData.Magnitude > 0.0f)
	// 	{
	// 		if (Data.Target.HasMatchingGameplayTag(EmberGameplayTag::Character_State_Invincible))
	// 		{
	// 			Data.EvaluatedData.Magnitude = 0.0f;
	// 			return false;
	// 		}
	// 	}
	// }
	
	return true;
}

void UEmberCharacterAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		EMBER_LOG(LogEmber,Warning,TEXT("Health: %f"), GetHealth());
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		EMBER_LOG(LogEmber,Log,TEXT("Damage: %f"), GetDamage());
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
		
		OnHit.Broadcast(Data.EffectSpec.GetContext().GetInstigator());
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		EMBER_LOG(LogEmber,Log,TEXT("Mana: %f"), GetMana());
		SetMana(FMath::Clamp(GetMana(), MinimumHealth, GetMaxMana()));
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		EMBER_LOG(LogEmber,Log,TEXT("Mana: %f"), GetShield());
		SetShield(FMath::Clamp(GetShield(), MinimumHealth, GetMaxShield()));
	}
	
	if (GetHealth() <= MinimumHealth && !bOutOfHealth)
	{
		//Data.Target.AddLooseGameplayTag(ABGameplayTag::Character_State_IsDead);
		OnOutOfHealth.Broadcast();
	}
	bOutOfHealth = (GetHealth() <= MinimumHealth);
	
	/* 마나랑 쉴드 브로드캐스트가 필요할지는 잘 모르겠음 */
}
