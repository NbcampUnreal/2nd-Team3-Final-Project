#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "EmberPlayerState.generated.h"

UCLASS()
class PROJECTEMBER_API AEmberPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AEmberPlayerState();

	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AbilitySystem", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<class UEmberCharacterAttributeSet> AttributeSet;
	UPROPERTY()
	TObjectPtr<class UEmberItemAttributeSet> ItemAttributeSet;
};
