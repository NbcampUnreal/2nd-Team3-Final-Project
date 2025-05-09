#include "Interactables/BaseInteractableActor.h"

#include "Character/EmberCharacter.h"
#include "Character/EmberComponents/InteractionComponent.h"
#include "Components/BoxComponent.h"

ABaseInteractableActor::ABaseInteractableActor()
{
 	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->InitBoxExtent(FVector(100.f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseInteractableActor::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ABaseInteractableActor::OnOverlapEnd);
}

void ABaseInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseInteractableActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEmberCharacter* Character = Cast<AEmberCharacter>(OtherActor))
	{
		Character->InteractionComponent->SetCurrentInteractable(this);
	}
}

void ABaseInteractableActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AEmberCharacter* Character = Cast<AEmberCharacter>(OtherActor))
	{
		Character->InteractionComponent->SetCurrentInteractable(nullptr);
	}
}

void ABaseInteractableActor::Interact_Implementation(AActor* Caller)
{
	IInteractable::Interact_Implementation(Caller);
}

