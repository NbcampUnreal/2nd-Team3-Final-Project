#include "LootActorBase.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Character/EmberCharacter.h"

ALootActorBase::ALootActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	
	SetRootComponent(MeshComponent);
}

// Called when the game starts or when spawned
void ALootActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALootActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALootActorBase::StartInteractAbility(APawn* InstigatorPawn)
{
	if (InstigatorPawn && InteractAbilityClass)
	{
		if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(InstigatorPawn))
		{
			if (UAbilitySystemComponent* AbilitySystem = EmberCharacter->GetAbilitySystemComponent())
			{
				FGameplayAbilitySpec* AbilitySpec = AbilitySystem->FindAbilitySpecFromClass(InteractAbilityClass);
				if (AbilitySpec->IsActive())
				{
				
				}
				else
				{
					EmberCharacter->SetOverlayMode(InteractOverlayTag);
					AbilitySystem->TryActivateAbility(AbilitySpec->Handle);
					TargetAbilitySystemComponent = AbilitySystem;
				}
			}
		}
	}
}

void ALootActorBase::UpdateInteractAbility()
{
	TargetAbilitySystemComponent->TryActivateAbilityByClass(InteractAbilityClass);
}

