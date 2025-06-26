#pragma once

#include "CoreMinimal.h"
#include "EmberUserWidget.h"
#include "GameplayEffectTypes.h"
#include "AnimalHpBarUserWidget.generated.h"

UCLASS()
class PROJECTEMBER_API UAnimalHpBarUserWidget : public UEmberUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHpBar() const;

	virtual void OnHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

	virtual void SetAbilitySystemComponent(class AActor* InOwner) override;
public:
	UFUNCTION(BlueprintCallable, Category = "HP")
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintCallable, Category = "HP")
	float GetCurrentMaxHealth() const;

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UProgressBar> PbHpBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TxtHpStat;
	
	UPROPERTY(BlueprintReadWrite)
	float CurrentAttributeHealth{100.0f};
	UPROPERTY(BlueprintReadWrite, SaveGame)
	float CurrentAttributeMaxHealth{100.0f};
};

inline float UAnimalHpBarUserWidget::GetCurrentHealth() const
{
	return CurrentAttributeHealth;
}

inline float UAnimalHpBarUserWidget::GetCurrentMaxHealth() const
{
	return CurrentAttributeMaxHealth;
}
