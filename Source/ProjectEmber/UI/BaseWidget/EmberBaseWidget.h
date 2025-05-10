#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmberBaseWidget.generated.h"

UENUM(BlueprintType)
enum class ELayer : uint8
{
	Game      UMETA(DisplayName="Game"),
	GameMenu  UMETA(DisplayName="GameMenu"),
	Menu      UMETA(DisplayName="Menu"),
	Modal     UMETA(DisplayName="Modal"),
};

UCLASS()
class PROJECTEMBER_API UEmberBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void Show();
	UFUNCTION(BlueprintCallable)
	virtual void Hide();
	UFUNCTION(BlueprintCallable)
	void Toggle();

protected:
	virtual void OnOpened() {}
	virtual void OnClosed() {}

	UPROPERTY(meta=(BindWidgetAnim))
	UWidgetAnimation* OpenAnim;
	UPROPERTY(meta=(BindWidgetAnim))
	UWidgetAnimation* CloseAnim;
	
	bool bIsVisible = false;
};
