#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmberPauseWidget.generated.h"

class UButton;
class UEmberSettingWidget;

UCLASS()
class PROJECTEMBER_API UEmberPauseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TSubclassOf<UUserWidget> SettingWidgetClass;

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnQuitClicked();
};