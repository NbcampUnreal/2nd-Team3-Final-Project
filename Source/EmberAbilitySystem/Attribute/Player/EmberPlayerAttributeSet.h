#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EMSActorSaveInterface.h"
#include "Attribute/EffectHelper/EmberEffectHelper.h"
#include "EmberPlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)	   \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)			   \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)			   \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)\

UCLASS(Config = Engine, DefaultConfig, Blueprintable)
class EMBERABILITYSYSTEM_API UEmberPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UEmberPlayerAttributeSet();
	void Initialize(UAbilitySystemComponent* InAsc);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public:
	ATTRIBUTE_ACCESSORS(UEmberPlayerAttributeSet, ParryDuration);
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess=true), SaveGame)
	FGameplayAttributeData ParryDuration;
	
protected: /* State Effects */

	void ApplyGameplayEffectToSelf(UAbilitySystemComponent* AbilitySystemComponent, const TSubclassOf<UGameplayEffect>& EffectClass, float Level = 1.0f);
	
	UPROPERTY(Config)
	TSubclassOf<UEmberEffectHelper> EffectHelperClass;

	UPROPERTY()
	UEmberEffectHelper* EffectHelperInstance;
};
