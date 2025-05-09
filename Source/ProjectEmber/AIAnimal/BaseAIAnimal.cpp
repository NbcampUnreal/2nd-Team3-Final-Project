#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "UI/EmberWidgetComponent.h"

ABaseAIAnimal::ABaseAIAnimal()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	CurrentState = EAnimalAIState::Idle;
	Personality = EAnimalAIPersonality::Normal;
	bIsShouldSwim = false;
	bIsHungry = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	CharacterAttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
	AnimalAttributeSet = CreateDefaultSubobject<UEmberAnimalAttributeSet>(TEXT("AnimalAttributeSet"));
	
	HpBarWidget = CreateDefaultSubobject<UEmberWidgetComponent>(TEXT("HpBarWidget"));
	HpBarWidget->SetupAttachment(GetMesh());
	HpBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
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
	
	if (HpBarWidgetClass)
	{
		HpBarWidget->SetWidgetClass(HpBarWidgetClass);
		HpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
		HpBarWidget->SetDrawSize(FVector2D(200.0f,20.0f));
		HpBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		HpBarWidget->UpdateAbilitySystemComponent();
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

EAnimalAIState ABaseAIAnimal::GetCurrentState() const
{
	return CurrentState;
}

void ABaseAIAnimal::SetCurrentState(EAnimalAIState NewState)
{
	CurrentState = NewState; //객체값 변경
	BlackboardComponent->SetValueAsEnum("CurrentState", static_cast<uint8>(CurrentState)); //블랙보드 갱신
	
}

UAbilitySystemComponent* ABaseAIAnimal::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

class UEmberCharacterAttributeSet* ABaseAIAnimal::GetCharacterAttributeSet() const
{
	return CharacterAttributeSet;
}

class UEmberAnimalAttributeSet* ABaseAIAnimal::GetAnimalAttributeSet() const
{
	return AnimalAttributeSet;
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
			//WalkSpeed *= 1.2f;
			break;
		}
	case EAnimalAIPersonality::Cowardly:
		{
			//WanderRange *= 1.2f;
			break;
		}
	case EAnimalAIPersonality::Lazy:
		{
			//WalkSpeed *= 0.8f;
			break;
		}
	case EAnimalAIPersonality::Outsider:
		{
			//WanderRange *= 1.2f;
			break;
		}
		default:
			break;
	}
}
