#include "EmberCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "GameplayTag/EmberGameplayTag.h"
#include "EmberLog/EmberLog.h"
#include "Utility/AlsGameplayTags.h"

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

void UEmberCharacterAttributeSet::Initialize(UAbilitySystemComponent* InAsc)
{
	EffectHelperInstance = NewObject<UEmberEffectHelper>(GetTransientPackage(), EffectHelperClass);
}

// 여기서 패링에대한 판단을 내려야할거같음
/*
 * 태그로 방어 중인지 판단
 * 즉 어빌리티베이스에서 방어인 친구는
 * 어트리뷰트에 패링시간(나중에 특성으로 패링판정 시간이 늘어날 수 있음)을 가져와서
 * 액티브 상태일때 바로 패링이라는 태그를 가지고 있고
 * 타이머로 패링 태그를 제거하는 방식으로 구현할 수 있을 것 같음
 *
 * 연타 방지용으로 0.1초 텀을 주는것도 나쁘지 않아 보임
 * 0.1초가 아니라 어빌리티 자체에 쿨타임을 0.1초 주는게 맞아보임
 */

void UEmberCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	//UAbilitySystemComponent* Asc = GetOwningAbilitySystemComponentChecked();
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
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponentChecked();
		if (AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::Parrying))
		{
			Data.EvaluatedData.Magnitude = 0.f;
			ApplyGameplayEffectToSelf(AbilitySystemComponent, EffectHelperInstance->ParryEffectClass, 1.0f);
		}
		else if (AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::Blocking))
		{
			// 방어 상태라면 반으로 줄이기
			Data.EvaluatedData.Magnitude *= 0.5f;
		}
	}
	
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

void UEmberCharacterAttributeSet::ApplyGameplayEffectToSelf(UAbilitySystemComponent* AbilitySystemComponent,
	const TSubclassOf<UGameplayEffect>& EffectClass, float Level)
{
	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	/* 나중에 들어온 Data로 커브값을 세팅해서 증가시키면 강도에 따라 마나 차는게 조절이 가능함*/
	const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, Level,Context);
	if (SpecHandle.IsValid())
	{
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}