#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EMSActorSaveInterface.h"
#include "EmberCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)	   \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)			   \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)			   \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfHealthDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHitMulticastDelegate, AActor*, Instigator);


UCLASS()
class EMBERABILITYSYSTEM_API UEmberCharacterAttributeSet : public UAttributeSet, public IEMSActorSaveInterface
{
	GENERATED_BODY()
public:
	UEmberCharacterAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public:
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, AttackRate);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, MaxAttackRate);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UEmberCharacterAttributeSet, Damage);
	
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

	bool bOutOfHealth{false};
};
