#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Character/InputHandler/EmberKeyMappingEntry.h"
#include "EmberKeySettingWidget.generated.h"

class UVerticalBox;
class UButton;
class UTextBlock;
class UInputMappingContext;
class UEmberKeyCaptureWidget;

UCLASS()
class PROJECTEMBER_API UEmberKeySettingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "KeyMapping")
    UInputMappingContext* EditableIMC = nullptr;
	
    UFUNCTION(BlueprintCallable)
    void SetTargetMappingContext(UInputMappingContext* InContext);

    UPROPERTY()
    TArray<FEmberDirectionalMoveEntry> CurrentMoveMappings;

    UPROPERTY()
    UInputMappingContext* ClonedIMC = nullptr;

    UPROPERTY(meta = (BindWidget))
    UButton* ForwardKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ForwardButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* LeftwardKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* LeftwardButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* RightwardKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RightwardButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* BackwardKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BackwardButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* ApplyButton;

    UPROPERTY(meta = (BindWidget))
    UButton* ResetButton;

    UFUNCTION()
    void UpdateKeyUI();

    UFUNCTION()
    void OnForwardKeyButtonClicked();

    UFUNCTION()
    void OnBackwardKeyButtonClicked();

    UFUNCTION()
    void OnLeftwardKeyButtonClicked();

    UFUNCTION()
    void OnRightwardKeyButtonClicked();

    UFUNCTION()
    void ShowKeyCaptureWidget();




    static EMoveDirection GetDirectionFromModifiers(const TArray<TObjectPtr<UInputModifier>>& Modifiers);

    UFUNCTION(BlueprintCallable)
    void LoadMappingsFromSave();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> KeyListContainer;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TSubclassOf<UEmberKeyCaptureWidget> KeyCaptureWidgetClass;

    TObjectPtr<UInputMappingContext> TargetMappingContext;

    TArray<FEmberKeyMappingEntry> CurrentMappings;

    TArray<FEmberKeyMappingEntry> DefaultMappings;

    TMap<UButton*, int32> KeyButtonMap;

    int32 PendingKeyChangeIndex = -1;

    UFUNCTION()
    void OnApplyClicked();

    UFUNCTION()
    void OnResetClicked();

    UFUNCTION()
    void OnKeyCaptured(FKey NewKey);

    EMoveDirection PendingKeyChangeDirection = EMoveDirection::Unknown;
};
