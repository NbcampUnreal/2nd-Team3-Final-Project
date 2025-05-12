#include "AreaTrigger.h"
#include "GameInstance/AudioSubsystem.h"
#include "GameInstance/EmberGameInstance.h"
#include "Character/EmberCharacter.h"

#include "Components/BoxComponent.h"

AAreaTrigger::AAreaTrigger()
{
 	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->InitBoxExtent(FVector(100.f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AAreaTrigger::OnOverlapBegin);
}

void AAreaTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEmberCharacter* Character = Cast<AEmberCharacter>(OtherActor))
	{
		if (AudioHandle.IsValid())
		{
			EMBER_LOG(LogTemp, Warning, TEXT("Overlap Event Success"));
			if (GI->CurrentAreaType != AreaType)
			{
				EMBER_LOG(LogTemp, Warning, TEXT("Play Bgm by Overlap"));
				AudioHandle->PlayBGMSoundByArea(AreaType);
				GI->CurrentAreaType = AreaType;
			}
		}
	}
}

void AAreaTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		GI = Cast<UEmberGameInstance>(GameInstance);
		if (GI.IsValid())
		{
			AudioHandle = GI->GetSubsystem<UAudioSubsystem>();
		}
	}
}
