#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Sound/SoundBase.h"
#include "EmberUIButton.generated.h"

UCLASS()
class PROJECTEMBER_API UEmberUIButton : public UButton
{
	GENERATED_BODY()

public:
	virtual void SynchronizeProperties() override;

protected:
	virtual void PostLoad() override;

	UPROPERTY(EditAnywhere, Category = "Ember|Sound")
	TObjectPtr<USoundBase> HoverSound;

	UPROPERTY(EditAnywhere, Category = "Ember|Sound")
	TObjectPtr<USoundBase> ClickSound;

	UFUNCTION()
	void OnHoveredInternal();

	UFUNCTION()
	void OnClickedInternal();

private:
	void PlayUISound(USoundBase* Sound);
	
};
