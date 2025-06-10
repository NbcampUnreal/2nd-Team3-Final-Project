#pragma once

#include "CoreMinimal.h"
#include "EmberUserWidget.h"
#include "GameplayEffectTypes.h"
#include "EmberHpBarUserWidget.generated.h"

UCLASS()
class PROJECTEMBER_API UEmberHpBarUserWidget : public UEmberUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHpBar() const;

	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

	virtual void OnManaChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxManaChanged(const FOnAttributeChangeData& ChangeData);

	virtual void OnShieldChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxShieldChanged(const FOnAttributeChangeData& ChangeData);
	
	virtual void SetAbilitySystemComponent(class AActor* InOwner) override;
public:
	UFUNCTION(BlueprintCallable, Category = "HP")
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable, Category = "HP")
	float GetCurrentMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Mana")
	float GetCurrentMana() const;
	UFUNCTION(BlueprintCallable, Category = "Mana")
	float GetCurrentMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Shield")
	float GetCurrentShield() const;
	UFUNCTION(BlueprintCallable, Category = "Shield")
	float GetCurrentMaxShield() const;
	
public:
	void OnInvincibleTagChanged(const FGameplayTag GameplayTag, int32 NewCount);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbHpBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbManaBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbShieldBar;

	UPROPERTY(BlueprintReadWrite, SaveGame)
	float CurrentAttributeHealth{100.0f};
	UPROPERTY(BlueprintReadWrite, SaveGame)
	float CurrentAttributeMaxHealth{100.0f};

	UPROPERTY(BlueprintReadWrite, SaveGame)
	float CurrentMana{100.0f};
	UPROPERTY(BlueprintReadWrite, SaveGame)
	float CurrentMaxMana{100.0f};
	
	UPROPERTY(BlueprintReadWrite, SaveGame)
	float CurrentShield{100.0f};
	UPROPERTY(BlueprintReadWrite, SaveGame)
	float CurrentMaxShield{100.0f};
};

inline float UEmberHpBarUserWidget::GetCurrentHealth() const
{
	return CurrentAttributeHealth;
}

inline float UEmberHpBarUserWidget::GetCurrentMaxHealth() const
{
	return CurrentAttributeMaxHealth;
}

inline float UEmberHpBarUserWidget::GetCurrentMana() const
{
	return CurrentMana;
}

inline float UEmberHpBarUserWidget::GetCurrentMaxMana() const
{
	return CurrentMaxMana;
}

inline float UEmberHpBarUserWidget::GetCurrentShield() const
{
	return CurrentShield;
}

inline float UEmberHpBarUserWidget::GetCurrentMaxShield() const
{
	return CurrentMaxShield;
}
