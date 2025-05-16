#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmberMainMenuWidget.generated.h"

class UButton;
class UEmberSettingWidget;

UCLASS()
class PROJECTEMBER_API UEmberMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnNewGameClicked();

	UFUNCTION()
	void OnContinueClicked();

	UFUNCTION()
	void OnSettingsClicked();

	UFUNCTION()
	void OnQuitClicked();

	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* SettingsButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	// 블루프린트에서 설정할 수 있는 SettingWidget 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UEmberSettingWidget> SettingWidgetClass;
};
