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
    int32 FindMappingIndexByName(FName MappingName) const;

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

    UPROPERTY(meta = (BindWidget))
    UButton* JumpKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* JumpButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* SprintKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* SprintButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* WalkKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* WalkButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* CrouchKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CrouchButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* BuildKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* BuildButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* InteractKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* InteractButtonText;

    UPROPERTY(meta = (BindWidget))
    UButton* GlideKeyButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GlideButtonText;

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
    void OnJumpKeyButtonClicked();

    UFUNCTION()
    void OnSprintKeyButtonClicked();

    UFUNCTION()
    void OnWalkKeyButtonClicked();

    UFUNCTION()
    void OnCrouchKeyButtonClicked();

    UFUNCTION()
    void OnBuildKeyButtonClicked();

    UFUNCTION()
    void OnInteractKeyButtonClicked();

    UFUNCTION()
    void OnGlideKeyButtonClicked();

    UFUNCTION()
    void ShowKeyCaptureWidget();

    UFUNCTION(BlueprintCallable)
    void LoadMappingsFromSave();

    void SetPendingMapping(FName MappingName);

    void UpdateActionKeyText(FName MappingName);

    FKey GetKeyForMapping(FName MappingName) const;


	static FString GetMoveDirectionMappingName(EMoveDirection Dir);

    static EMoveDirection GetDirectionFromModifiers(const TArray<TObjectPtr<UInputModifier>>& Modifiers);



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
