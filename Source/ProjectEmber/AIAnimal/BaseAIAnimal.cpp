#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "NavigationInvokerComponent.h"
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
	NavGenerationRadius = 4000.0f; //시각,청각 인지 버뮈보다 인보커 생성 범위가 커야함
	NavRemovalRadius = 4300.0f;
	//NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>("NavInvokerComponent");


	
	bIsShouldSwim = false;
  
	CurrentState = EAnimalAIState::Idle;
	GenerateRandom();

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent::성공"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent::실패."));
	}
	
	CharacterAttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
	AnimalAttributeSet = CreateDefaultSubobject<UEmberAnimalAttributeSet>(TEXT("AnimalAttributeSet"));
	
	HpBarWidget = CreateDefaultSubobject<UEmberWidgetComponent>(TEXT("HpBarWidget"));
	HpBarWidget->SetupAttachment(GetMesh());
	HpBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
}

void ABaseAIAnimal::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitStats(UEmberCharacterAttributeSet::StaticClass(), nullptr);
	AbilitySystemComponent->InitStats(UEmberAnimalAttributeSet::StaticClass(), nullptr);
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
	
	GetWorldTimerManager().SetTimer(TimerHandle, this,&ABaseAIAnimal::DecreaseFullness,5.0f,true);

	//NavInvokerComponent->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);
}

void ABaseAIAnimal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseAIAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

void ABaseAIAnimal::DecreaseFullness()
{
	Fullness -= 1.f;
	Fullness = FMath::Clamp(Fullness, 0.0f, 100.0f);
	if (bIsHungry == false && Fullness <= 50.0f)
	{
		bIsHungry = true;
		BlackboardComponent->SetValueAsBool("IsHungry", bIsHungry);
	}
	UE_LOG(LogTemp, Warning, TEXT("DecreaseFullness :: Fullness , IsHungry %f, %d") ,Fullness, bIsHungry);
}
void ABaseAIAnimal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 타이머 해제
	GetWorldTimerManager().ClearTimer(TimerHandle);
}

float ABaseAIAnimal::GetWildPower() const
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

UAbilitySystemComponent* ABaseAIAnimal::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

const class UEmberCharacterAttributeSet* ABaseAIAnimal::GetCharacterAttributeSet() const
{
	
	return AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>();
}

const class UEmberAnimalAttributeSet* ABaseAIAnimal::GetAnimalAttributeSet() const
{
	return AbilitySystemComponent->GetSet<UEmberAnimalAttributeSet>();
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
	return nullptr;//NavInvokerComponent;
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
