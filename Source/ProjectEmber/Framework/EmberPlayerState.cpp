#include "EmberPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Item/Ability/EmberItemAttributeSet.h"

AEmberPlayerState::AEmberPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("AttributeSet"));
	ItemAttributeSet = CreateDefaultSubobject<UEmberItemAttributeSet>(TEXT("ItemAttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject<UEmberCharacterAttributeSet>(AttributeSet);
	AbilitySystemComponent->AddAttributeSetSubobject<UEmberItemAttributeSet>(ItemAttributeSet);

	AbilitySystemComponent->SetIsReplicated(true);
}

class UAbilitySystemComponent* AEmberPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

