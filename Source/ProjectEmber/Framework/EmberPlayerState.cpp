#include "EmberPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"

AEmberPlayerState::AEmberPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("AttributeSet"));
	
	AbilitySystemComponent->SetIsReplicated(true);
}

class UAbilitySystemComponent* AEmberPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

