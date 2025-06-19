#include "TutorialManagerSubsystem.h"
#include "Tutorial/Widget/TutorialWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Tutorial/DataAssest/TutorialDataAsset.h"

UTutorialManagerSubsystem::UTutorialManagerSubsystem()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/_NPCAI/Widgets/WBP_Tutorial.WBP_Tutorial_C"));
    if (WidgetBPClass.Succeeded())
    {
        TutorialWidgetClass = WidgetBPClass.Class;
    }
}

void UTutorialManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    OnTutorialTriggered.AddDynamic(this, &UTutorialManagerSubsystem::HandleTutorialTriggered);
}

void UTutorialManagerSubsystem::HandleTutorialTriggered(const FTutorialData& TutorialData)
{
    ShowTutorial(TutorialData);
}

void UTutorialManagerSubsystem::ShowTutorial(const FTutorialData& TutorialData)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    if (!TutorialWidget && TutorialWidgetClass)
    {
        TutorialWidget = CreateWidget<UTutorialWidget>(PC, TutorialWidgetClass);
        if (TutorialWidget)
        {
            TutorialWidget->AddToViewport();
        }
    }

    if (TutorialWidget)
    {
        TutorialWidget->SetTutorialData(TutorialData);
        TutorialWidget->PlayAnimation(TutorialWidget->BoderAnimation);
        TutorialWidget->ShowTutorial(true);

        if (TutorialData.Sound)
        {
            UGameplayStatics::PlaySound2D(GetWorld(), TutorialData.Sound);
        }

        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);
        PC->SetShowMouseCursor(true);
        PC->SetInputMode(FInputModeUIOnly());
    }
}

void UTutorialManagerSubsystem::HideTutorial()
{
    if (TutorialWidget)
    {
        TutorialWidget->ShowTutorial(false);
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        PC->SetIgnoreMoveInput(false);
        PC->SetIgnoreLookInput(false);
        PC->SetShowMouseCursor(false);
        PC->SetInputMode(FInputModeGameOnly());
    }
}