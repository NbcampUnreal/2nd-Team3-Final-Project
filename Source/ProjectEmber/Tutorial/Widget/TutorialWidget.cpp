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
}

void UTutorialWidget::ShowTutorial(bool bShow)
{
    SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UTutorialWidget::SetTutorialData(const FTutorialData& InData)
{
    SetImageTexture(InData.KeyImage);
    SetMediaImageTexture(InData.VideoTexture);
    if (TutorialNameText)
    {
        TutorialNameText->SetText(InData.Name);
    }

    if (TutorialDescriptionText)
    {
        TutorialDescriptionText->SetText(InData.Description);
    }
}

void UTutorialWidget::OnCloseButtonClicked()
{
    //  Subsystem ã�Ƽ� HideTutorial ȣ��!
    if (UTutorialManagerSubsystem* TutorialSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTutorialManagerSubsystem>())
    {
        TutorialSubsystem->HideTutorial();
    }
}