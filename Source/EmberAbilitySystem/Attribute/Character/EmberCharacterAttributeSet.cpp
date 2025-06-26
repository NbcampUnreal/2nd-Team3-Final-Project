#include "EmberCharacterAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AlsCharacter.h"
#include "GameplayEffectExtension.h"
#include "Ability/Combat/ParryCounterAbility.h"
#include "GameplayTag/EmberGameplayTag.h"
#include "EmberLog/EmberLog.h"
#include "FunctionLibrary/CombatFunctionLibrary.h"
#include "Utility/AlsGameplayTags.h"

UEmberCharacterAttributeSet::UEmberCharacterAttributeSet()
{
	InitAttackRate(30.0f);
	InitMaxAttackRate(100.0f);

	InitHealth(100.0f);
	InitMaxHealth(100.0f);
	
	InitDamage(0.0f);

	InitMana(10.0f);
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
		if (AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::Invincibility))
		{
			Data.EvaluatedData.Magnitude = 0.f;
			return true;
		}
		/*
		* 극한회피 슬로우 0.35초 페이드 인 0.12초
		* 패링 슬로우 0.17초 이펙트가 끝나는 시간 0.17초
		* 다시 행동가능한 시간으로 돌아오는 시간 0.15초? 0.17?
		*/
		
		if (AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::Parrying))
		{
			EMBER_LOG(LogEmber, Warning, TEXT("Parrying!"));
			// 1. 시간을 느리게 
			UCombatFunctionLibrary::ApplyGlobalTimeDilation(GetWorld(), 0.4f,0.17f);
			// 2. 데미지 무효화
			Data.EvaluatedData.Magnitude = 0.f;
			// 3. 패링 카운터 어빌리티 발동 (상대에게)
			const FGameplayEffectContextHandle& Context = Data.EffectSpec.GetContext();
			if (UAbilitySystemComponent* SourceAsc = Context.GetInstigatorAbilitySystemComponent())
			{
				bool bCan = SourceAsc->TryActivateAbilityByClass(EffectHelperInstance->EnemyParryAbilityClass);
				if (!bCan)
				{
					EMBER_LOG(LogEmber, Warning, TEXT("Failed to activate enemy parry ability"));
				}
			}
			// 4. 패링 카운터 어빌리티 발동 (나에게)
			AbilitySystemComponent->TryActivateAbilityByClass(EffectHelperInstance->ParryAbilityClass);
		}
		else if (AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::Blocking))
		{
			Data.EvaluatedData.Magnitude *= 0.8f;

			const FGameplayEffectContextHandle& Context = Data.EffectSpec.GetContext();

			AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(EffectHelperInstance->BlockHitEffectClass, 1.0f, Context);
			
			FGameplayEventData Payload;
			Payload.EventTag = AlsCharacterStateTags::Hit;
			Payload.Instigator = Context.GetInstigator();
			Payload.Target = GetOwningActor();
	
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwningActor(),	AlsCharacterStateTags::Hit,Payload);
		}
		else // 이쪽은 무조건 Hit 어빌리티 발동시켜야됨
		{
			DirectionalHitAbility(Data);
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

		/* 테스트 코드
		 * 포스트로 처리할지 이펙트로 처리할지 고민
		 */
		float CurrentDamage = GetDamage();
		UAbilitySystemComponent* AbilitySystemComponent = GetOwningAbilitySystemComponentChecked();
		if (AbilitySystemComponent->HasMatchingGameplayTag(AlsCharacterStateTags::Blocking))
		{
			CurrentDamage = FMath::Clamp(GetDamage() - GetShield(), MinimumHealth, GetMaxShield());
			SetShield(FMath::Clamp(GetShield() - GetDamage(), MinimumHealth, GetMaxHealth()));
		}
		
		SetHealth(FMath::Clamp(GetHealth() - CurrentDamage, MinimumHealth, GetMaxHealth()));
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

void UEmberCharacterAttributeSet::DirectionalHitAbility(const FGameplayEffectModCallbackData& Data)
{
	UAbilitySystemComponent* Asc = GetOwningAbilitySystemComponentChecked();
	if (!Asc)
	{
		return;
	}

	const AAlsCharacter* Character = Cast<AAlsCharacter>(Asc->GetAvatarActor());
	if (!Character)
	{
		return;
	}
	
	const FGameplayEffectContextHandle& Context = Data.EffectSpec.GetContext();
	const AActor* Instigator = Context.GetInstigator();
	if (!Instigator)
	{
		return;
	}

	// 1. 방향 벡터 계산
	const FVector ToInst   = (Instigator->GetActorLocation() - Character->GetActorLocation()).GetSafeNormal();
	const FVector Forward  = Character->GetActorForwardVector();
	const FVector Right    = Character->GetActorRightVector();

	// 2. 내적으로 방향 계산
	const float ForwardDot = FVector::DotProduct(Forward, ToInst);
	const float RightDot   = FVector::DotProduct(Right, ToInst);
	
	TSubclassOf<UGameplayAbility> AbilityToActivate;
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		// 전후
		if (ForwardDot > 0.f)
			AbilityToActivate = EffectHelperInstance->ForwardHitAbilityClass;
		else
			AbilityToActivate = EffectHelperInstance->BackHitAbilityClass;
	}
	else
	{
		// 좌우
		if (RightDot > 0.f)
			AbilityToActivate = EffectHelperInstance->RightHitAbilityClass;
		else
			AbilityToActivate = EffectHelperInstance->LeftHitAbilityClass;
	}
	
	if (AbilityToActivate)
	{
		Asc->TryActivateAbilityByClass(AbilityToActivate);
	}
}
