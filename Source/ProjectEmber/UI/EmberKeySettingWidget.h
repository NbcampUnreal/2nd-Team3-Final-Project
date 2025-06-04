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

    static EMoveDirection GetDirectionFromModifiers(const TArray<TObjectPtr<UInputModifier>>& Modifiers);

    UFUNCTION(BlueprintCallable)
    void LoadMappingsFromSave();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> KeyListContainer;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ApplyButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ResetButton;

    // Ű �Է� ĸó ���� Ŭ����
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TSubclassOf<UEmberKeyCaptureWidget> KeyCaptureWidgetClass;

    TObjectPtr<UInputMappingContext> TargetMappingContext;

    TArray<FEmberKeyMappingEntry> CurrentMappings;

    TArray<FEmberKeyMappingEntry> DefaultMappings;

    TMap<UButton*, int32> KeyButtonMap;

    int32 PendingKeyChangeIndex = -1;

    void RefreshKeyList();

    UFUNCTION()
    void OnApplyClicked();

    UFUNCTION()
    void OnResetClicked();

    UFUNCTION()
    void OnAnyKeyButtonClicked();

    UFUNCTION()
    void OnKeyCaptured(FKey NewKey);

    void OnKeyButtonClicked(int32 MappingIndex);

	FString GetMoveDirectionMappingName(EMoveDirection Dir);
};
