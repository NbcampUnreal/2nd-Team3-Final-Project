#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmberVideoSettingPanel.generated.h"

UCLASS()
class PROJECTEMBER_API UEmberVideoSettingPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void ApplyVideoSettings();

protected:
	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* ScreenModeComboBox;

	UPROPERTY(meta = (BindWidget))
	class UComboBoxString* ResolutionComboBox;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* OverallComboBox;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* TextureComboBox;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* ShadowComboBox;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* EffectsComboBox;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* AAComboBox;

    UPROPERTY(meta = (BindWidget))
    class UComboBoxString* ViewDistanceComboBox;

    UPROPERTY(meta = (BindWidget))
    class UButton* ApplyButton;

private:
    void InitOptions();

    int32 GetQualityIndexFromString(const FString& QualityName) const;

};
