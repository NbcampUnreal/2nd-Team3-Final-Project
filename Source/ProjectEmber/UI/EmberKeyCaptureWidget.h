#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputCoreTypes.h"
#include "EmberKeyCaptureWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyCaptured, FKey, CapturedKey);

UCLASS()
class PROJECTEMBER_API UEmberKeyCaptureWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UEmberKeyCaptureWidget(const FObjectInitializer& ObjectInitializer);

    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    UPROPERTY(BlueprintAssignable, Category = "Input")
    FOnKeyCaptured OnKeyCaptured;

protected:
    virtual void NativeConstruct() override;
};
