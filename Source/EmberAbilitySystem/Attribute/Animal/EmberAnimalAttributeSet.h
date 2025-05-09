#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EmberAnimalAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class EMBERABILITYSYSTEM_API UEmberAnimalAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UEmberAnimalAttributeSet();
    
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public: /* Behavior Tree Variable */
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, BumperRange)
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, WarningRange)
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, WalkSpeed)
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, WanderRange)
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, WildPower)
    
protected: 
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData BumperRange;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData WarningRange;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData WalkSpeed;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData WanderRange;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData WildPower;
};
