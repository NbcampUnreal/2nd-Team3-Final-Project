#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EMSActorSaveInterface.h"
#include "Attribute/EffectHelper/EmberEffectHelper.h"
#include "EmberCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)	   \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)			   \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)			   \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)\

struct FEmberCharacterAttributeSetSaveData;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOutOfHealthDelegate, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitMulticastDelegate, AActor*, Instigator);


UCLASS(Config = Engine, DefaultConfig, Blueprintable)
class EMBERABILITYSYSTEM_API UEmberCharacterAttributeSet : public UAttributeSet, public IEMSActorSaveInterface
{
	GENERATED_BODY()
public:
	UEmberCharacterAttributeSet();
	void Initialize(UAbilitySystemComponent* InAsc);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

protected:
	
public:
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, AttackRate);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, MaxAttackRate);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, Damage);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, Mana);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, MaxMana);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, Shield);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, MaxShield);
	
	mutable FOutOfHealthDelegate OnOutOfHealth;

	UPROPERTY(BlueprintAssignable)
	FHitMulticastDelegate OnHit;
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData AttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData MaxAttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData Health;
	
	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Health", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Mana", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData Mana;
	
	UPROPERTY(BlueprintReadOnly, Category = "Mana", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData MaxMana;

	UPROPERTY(BlueprintReadOnly, Category = "Shield", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData Shield;
	
	UPROPERTY(BlueprintReadOnly, Category = "Shield", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData MaxShield;

	bool bOutOfHealth{false};

protected: /* State Effects */

	void ApplyGameplayEffectToSelf(UAbilitySystemComponent* AbilitySystemComponent, const TSubclassOf<UGameplayEffect>& EffectClass, float Level = 1.0f);
	void DirectionalHitAbility(const FGameplayEffectModCallbackData& Data);
	
	UPROPERTY(Config)
	TSubclassOf<UEmberEffectHelper> EffectHelperClass;

	UPROPERTY()
	UEmberEffectHelper* EffectHelperInstance;

public: /* Save Data */
	void FillSaveData(FEmberCharacterAttributeSetSaveData& OutData) const;
	void LoadSaveData(const FEmberCharacterAttributeSetSaveData& InData);
};

