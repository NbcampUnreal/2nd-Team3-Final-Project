#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "NavigationInvokerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

ABaseAIAnimal::ABaseAIAnimal()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	NavGenerationRadius = 4000.0f; //시각,청각 인지 버뮈보다 인보커 생성 범위가 커야함
	NavRemovalRadius = 4300.0f;
	NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>("NavInvokerComponent");
	NavInvokerComponent->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);

	
	bIsShouldSwim = false;
	CurrentState = EAnimalAIState::Idle;
	GenerateRandom();
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
	//방법1 : 일정주기마다 무조건 배고픔 활성화 
	//GetWorldTimerManager().SetTimer(TimerHandle, this,&ABaseAIAnimal::SetFullness,5.0f,true); 
}

void ABaseAIAnimal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseAIAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Fullness -= 0.1f;
	Fullness = FMath::Clamp(Fullness, 0.0f, 100.0f);
	if (bIsHungry == false && Fullness <= 50.0f)
	{
		bIsHungry = true;
		BlackboardComponent->SetValueAsBool("IsHungry", bIsHungry);
	}
	UE_LOG(LogTemp, Warning, TEXT("Tick :: Fullness , IsHungry %f, %d") ,Fullness, bIsHungry);
}

void ABaseAIAnimal::SetFullness()
{
	bIsHungry = !bIsHungry;
	if (!bIsHungry)
	{
		Fullness = 100.0f;
		UE_LOG(LogTemp, Warning, TEXT("SetFullness :: Fullness , IsHungry %f, %d") ,Fullness, bIsHungry);
	}
	BlackboardComponent->SetValueAsBool("IsHungry", bIsHungry);
}

void ABaseAIAnimal::GenerateRandom()
{
	int32 RandomPersonality = FMath::RandRange(0, static_cast<int32>(EAnimalAIPersonality::End) - 1);
	Personality = static_cast<EAnimalAIPersonality>(RandomPersonality);
	Fullness = FMath::FRandRange(0.f, 100.f);
	bIsHungry = Fullness <= 50.f;
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
