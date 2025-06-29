#include "TutorialManagerSubsystem.h"
#include "Tutorial/Widget/TutorialWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

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
    ShowTutorialSequence({ TutorialData });
}

void UTutorialManagerSubsystem::ShowTutorialSequence(const TArray<FTutorialData>& Sequence)
{
    if (Sequence.Num() == 0) return;

    CurrentTutorialSequence = Sequence;
    CurrentTutorialIndex = 0;
    ShowTutorial(CurrentTutorialSequence[0]);
}

void UTutorialManagerSubsystem::ShowTutorial(const FTutorialData& TutorialData)
{
    bIsTutorialActive = true;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    if (!TutorialWidget && TutorialWidgetClass)
    {
        TutorialWidget = CreateWidget<UTutorialWidget>(PC, TutorialWidgetClass);
        if (IsValid(TutorialWidget))
        {
            TutorialWidget->AddToViewport();
        }
    }

    if (IsValid(TutorialWidget))
    {
        TutorialWidget->SetTutorialData(
            TutorialData,
            CurrentTutorialIndex,
            CurrentTutorialSequence.Num()
        );

        TutorialWidget->PlayAnimation(TutorialWidget->BoderAnimation);
        TutorialWidget->ShowTutorial(true);

        if (TutorialData.Sound)
        {
            UGameplayStatics::PlaySound2D(GetWorld(), TutorialData.Sound);
        }
        
        if (CurrentTutorialIndex == 0)
        {
            PC->SetShowMouseCursor(true);
            PC->SetIgnoreMoveInput(true);
            PC->SetIgnoreLookInput(true);
            PC->SetInputMode(FInputModeUIOnly());
        }
    }
}

void UTutorialManagerSubsystem::OnNextTutorialRequested()
{
    CurrentTutorialIndex++;

    if (CurrentTutorialSequence.IsValidIndex(CurrentTutorialIndex))
    {
        ShowTutorial(CurrentTutorialSequence[CurrentTutorialIndex]);
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
        UE_LOG(LogTemp, Warning, TEXT("[Tutorial] MoveInput Ignored: %s"), PC->IsMoveInputIgnored() ? TEXT("True") : TEXT("False"));
        UE_LOG(LogTemp, Warning, TEXT("[Tutorial] LookInput Ignored: %s"), PC->IsLookInputIgnored() ? TEXT("True") : TEXT("False"));
        UE_LOG(LogTemp, Warning, TEXT("[Tutorial] SetInputMode: GameOnly"));
    }

    CurrentTutorialSequence.Empty();
    CurrentTutorialIndex = 0;
    bIsTutorialActive = false;
}
