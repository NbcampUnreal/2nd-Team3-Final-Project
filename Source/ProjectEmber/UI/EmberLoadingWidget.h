#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "EmberLoadingWidget.generated.h"

UCLASS()
class PROJECTEMBER_API UEmberLoadingWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* LoadingProgressBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loading")
	float Progress = 0.0f;

};
