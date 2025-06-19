#include "TutorialBoxTrigger.h"
#include "Components/BoxComponent.h"
#include "Tutorial/Subsystem/TutorialManagerSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ATutorialBoxTrigger::ATutorialBoxTrigger()
{
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    RootComponent = BoxComponent;

    BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATutorialBoxTrigger::OnOverlapBegin);
}

void ATutorialBoxTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!Cast<APawn>(OtherActor) || bTriggered)
    {
        return;
    }
    if (Cast<APawn>(OtherActor) && TutorialDataAsset)
    {
        if (TutorialDataAsset->Tutorials.IsValidIndex(TutorialIndex))
        {
            if (UTutorialManagerSubsystem* TutorialSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTutorialManagerSubsystem>())
            {
                TutorialSubsystem->OnTutorialTriggered.Broadcast(
                    TutorialDataAsset->Tutorials[TutorialIndex]
                );
                bTriggered = true;
            }
        }
    }
}