#include "TutorialWidget.h"

#include "MediaTexture.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"   
#include "Tutorial/Subsystem/TutorialManagerSubsystem.h"

void UTutorialWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    SetVisibility(ESlateVisibility::Hidden);

    if (TutorialButton)
    {
        TutorialButton->OnClicked.AddDynamic(this, &UTutorialWidget::OnCloseButtonClicked);
    }
    if (NextButton)
    {
        NextButton->OnClicked.AddDynamic(this, &UTutorialWidget::OnNextButtonClicked);
    }
}

void UTutorialWidget::SetImageTexture(UTexture2D* NewImage)
{
    if (KeylImage && NewImage)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(NewImage);
        Brush.ImageSize = FVector2D(NewImage->GetSizeX(), NewImage->GetSizeY());
        KeylImage->SetBrush(Brush);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("KeyImage or NewImage is invalid in SetImageTexture"));
    }
}

void UTutorialWidget::SetMediaImageTexture(UTexture* NewImage)
{
    if (MediaImage && NewImage)
    {
        FSlateBrush Brush;
        Brush.SetResourceObject(NewImage);
        Brush.ImageSize = FVector2D(100, 100); // 적절한 크기로 설정
        MediaImage->SetBrush(Brush);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("KeyImage or NewImage is invalid in SetMediaImageTexture"));
    }
}

void UTutorialWidget::ShowTutorial(bool bShow)
{
    SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UTutorialWidget::SetTutorialData(const FTutorialData& Data, int32 CurrentIndex, int32 TotalCount)
{
    SetImageTexture(Data.KeyImage);
    SetMediaImageTexture(Data.VideoTexture);

    if (TutorialNameText)
    {
        TutorialNameText->SetText(Data.Name);
    }

    if (TutorialDescriptionText)
    {
        TutorialDescriptionText->SetText(Data.Description);
    }

    // Next 버튼 보이기 제어
    if (NextButton)
    {
        const bool bHasNext = CurrentIndex < TotalCount - 1;
        NextButton->SetVisibility(bHasNext ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UTutorialWidget::OnCloseButtonClicked()
{
    if (UTutorialManagerSubsystem* TutorialSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTutorialManagerSubsystem>())
    {
        TutorialSubsystem->HideTutorial();
    }
}
void UTutorialWidget::OnNextButtonClicked()
{
    if (UTutorialManagerSubsystem* TutorialSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTutorialManagerSubsystem>())
    {
        TutorialSubsystem->OnNextTutorialRequested();
    }
}