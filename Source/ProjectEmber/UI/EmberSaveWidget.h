#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmberSaveWidget.generated.h"

class UButton;

UCLASS()
class PROJECTEMBER_API UEmberSaveWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnBackClicked();

    UPROPERTY(meta = (BindWidget))
    UButton* BTN_Back;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> MainMenuWidgetClass;

};
