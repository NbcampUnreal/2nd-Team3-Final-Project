#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseAIAnimal::ABaseAIAnimal()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	CurrentState = EAnimalAIState::Idle;
	Personality = EAnimalAIPersonality::Normal;
	bIsShouldSwim = false;
	bIsHungry = false;
}

void ABaseAIAnimal::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIAnimalController>(GetController());
	if (AIController)
	{
		BlackboardComponent = AIController->GetBlackboardComponent();
		if (!BlackboardComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("ABaseAIAnimal::블랙보드 초기화 실패."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABaseAIAnimal::AIController 초기화 실패."));
	}
}

void ABaseAIAnimal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseAIAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ABaseAIAnimal::GetWarningRange() const
{
	return WarningRange;
}

float ABaseAIAnimal::GetWanderRange() const
{
	return WanderRange;
}

int32 ABaseAIAnimal::GetWildPower() const
{
	return WildPower;
}

EAnimalAIState ABaseAIAnimal::GetCurrentState() const
{
	return CurrentState;
}

void ABaseAIAnimal::SetCurrentState(EAnimalAIState NewState)
{
	CurrentState = NewState; //객체값 변경
	BlackboardComponent->SetValueAsEnum("CurrentState", static_cast<uint8>(CurrentState)); //블랙보드 갱신
	
}

EAnimalAIPersonality ABaseAIAnimal::GetPersonality() const
{
	return Personality;
}

void ABaseAIAnimal::PlayInteractMontage(uint8 InState)
{
	if (Montage)
	{
		PlayAnimMontage(Montage, 1.0f);
	}
}

UNavigationInvokerComponent* ABaseAIAnimal::GetNavInvoker() const
{
	return NavInvokerComponent;
}

void ABaseAIAnimal::SetDetails()
{
	switch(Personality)
	{
	case EAnimalAIPersonality::Agile:
		{
			WalkSpeed *= 1.2f;
			break;
		}
	case EAnimalAIPersonality::Cowardly:
		{
			WanderRange *= 1.2f;
			break;
		}
	case EAnimalAIPersonality::Lazy:
		{
			WalkSpeed *= 0.8f;
			break;
		}
	case EAnimalAIPersonality::Outsider:
		{
			WanderRange *= 1.2f;
			break;
		}
		default:
			break;
	}
}
