#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EMSActorSaveInterface.h"
#include "EmberAnimalAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT(BlueprintType)
struct FEmberAnimalAttributeData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Fullness = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WalkSpeed = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WanderRange = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WildPower = 0.f;
};

UCLASS()
class EMBERABILITYSYSTEM_API UEmberAnimalAttributeSet : public UAttributeSet, public IEMSActorSaveInterface
{
    GENERATED_BODY()

public:
    UEmberAnimalAttributeSet();
    
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
    void InitFromData(const FEmberAnimalAttributeData& Data);

public: /* Behavior Tree Variable */
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, Fullness)
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, WalkSpeed)
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, WanderRange)
    ATTRIBUTE_ACCESSORS(UEmberAnimalAttributeSet, WildPower)
    
protected: 
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true), SaveGame)
    FGameplayAttributeData Fullness;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true), SaveGame)
    FGameplayAttributeData WalkSpeed;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true), SaveGame)
    FGameplayAttributeData WanderRange;
    
    UPROPERTY(BlueprintReadOnly, Category = "Animal", Meta = (AllowPrivateAccess = true), SaveGame)
    FGameplayAttributeData WildPower;
};
