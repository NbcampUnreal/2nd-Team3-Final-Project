#include "TutorialBoxTrigger.h"
#include "Components/BoxComponent.h"
#include "Tutorial/Subsystem/TutorialManagerSubsystem.h"
#include "GameFramework/PlayerController.h"
 

ATutorialBoxTrigger::ATutorialBoxTrigger()
{
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    RootComponent = BoxComponent;
    BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATutorialBoxTrigger::OnOverlapBegin);
}

void ATutorialBoxTrigger::BeginPlay()
{
    Super::BeginPlay();
}

void ATutorialBoxTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!Cast<APawn>(OtherActor))
        return;

    if (bTriggered)
    {
        UE_LOG(LogTemp, Warning, TEXT("▶ 튜토리얼 트리거가 이미 실행되었습니다: %s"), *GetName());
        return;
    }

    UTutorialManagerSubsystem* TutorialSubsystem = GetGameInstance()->GetSubsystem<UTutorialManagerSubsystem>();
    if (!TutorialSubsystem || !TutorialDataAsset)
        return;

    TArray<FTutorialData> TutorialSequence;

    for (int32 Index : TutorialIndexs)
    {
        if (TutorialDataAsset->Tutorials.IsValidIndex(Index))
        {
            TutorialSequence.Add(TutorialDataAsset->Tutorials[Index]);
        }
    }

    if (TutorialSequence.Num() > 0)
    {
        TutorialSubsystem->ShowTutorialSequence(TutorialSequence);
        bTriggered = true;
    }
}



