// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EmberUserWidget.h"
#include "GameplayEffectTypes.h"
#include "EmberHpBarUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberHpBarUserWidget : public UEmberUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHpBar() const;

	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	
protected:
	virtual void SetAbilitySystemComponent(class AActor* InOwner) override;
	
	void OnInvincibleTagChanged(const FGameplayTag GameplayTag, int32 NewCount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbHpBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtHpStat;

	UPROPERTY(SaveGame)
	float CurrentHealth{0.0f};
	UPROPERTY(SaveGame)
	float CurrentMaxHealth{0.1f};

	FLinearColor HealthColor{FLinearColor::Red};
	FLinearColor InvincibleColor{FLinearColor::Blue};
};
