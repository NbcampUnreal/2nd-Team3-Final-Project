#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "NavigationInvokerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "EMSFunctionLibrary.h"
#include "GameplayEffectExtension.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Components/BoxComponent.h"
#include "EmberLog/EmberLog.h"
#include "UI/EmberHpBarUserWidget.h"
#include "UI/EmberWidgetComponent.h"

ABaseAIAnimal::ABaseAIAnimal()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	NavGenerationRadius = 4000.0f; //시각,청각 인지 버뮈보다 인보커 생성 범위가 커야함
	NavRemovalRadius = 4300.0f;
	NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>("NavInvokerComponent");

	bIsShouldSwim = false;
	CurrentState = EAnimalAIState::Idle;
	GenerateRandom();

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
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
	}

	if (HpBarWidgetClass)
	{
		HpBarWidget->SetWidgetClass(HpBarWidgetClass);
		HpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
		HpBarWidget->SetDrawSize(FVector2D(200.0f, 20.0f));
		HpBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		HpBarWidget->UpdateAbilitySystemComponent();
	}
	NavInvokerComponent->SetGenerationRadii(NavGenerationRadius, NavRemovalRadius);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseAIAnimal::DecreaseFullness, 5.0f, true);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetHealthAttribute()).
	                        AddUObject(this, &ThisClass::OnHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UEmberCharacterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

	if (const UEmberCharacterAttributeSet* Attribute = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
	{
		const_cast<UEmberCharacterAttributeSet*>(Attribute)->OnHit.AddDynamic(this, &ABaseAIAnimal::OnHit);
	}

	//test
	BlackboardComponent->SetValueAsEnum("Personality", static_cast<uint8>(Personality));
}

void ABaseAIAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseAIAnimal::ActorPreSave_Implementation()
{
	IEMSActorSaveInterface::ActorPreSave_Implementation();

	if (CharacterAttributeSet)
	{
		FRawObjectSaveData SaveData;
		SaveData.Id = TEXT("AttributeSet_Character");
		SaveData.Object = CharacterAttributeSet;
		UEMSFunctionLibrary::SaveRawObject(this, SaveData);
	}
}

void ABaseAIAnimal::ActorLoaded_Implementation()
{
	IEMSActorSaveInterface::ActorLoaded_Implementation();
	
	if (CharacterAttributeSet)
	{
		FRawObjectSaveData SaveData;
		SaveData.Id = TEXT("AttributeSet_Character");
		SaveData.Object = CharacterAttributeSet;
		UEMSFunctionLibrary::LoadRawObject(this, SaveData);

		FOnAttributeChangeData ChangeData;
		ChangeData.NewValue = CharacterAttributeSet->GetHealth();
		Cast<UEmberHpBarUserWidget>(HpBarWidget->GetWidget())->OnHealthChanged(ChangeData);
		EMBER_LOG(LogEmber, Warning,TEXT("%f"),CharacterAttributeSet->GetHealth()); 
	}
}

void ABaseAIAnimal::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT(" ABaseAIAnimal::OnHealthChanged::성공"));
	//속도 빨라졌다 서서히 감소 추가해야함
	BlackboardComponent->SetValueAsBool("IsRest", false);
	BlackboardComponent->SetValueAsBool("IsHit", true);
	//여기서 타겟오브젝트 설정 -> 이미  IsRest, IsHit가 값이 위에처럼 설정되면 다른 노드로 들어가지 않음
}

void ABaseAIAnimal::OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseAIAnimal::OnMaxHealthChanged::성공"));
}


void ABaseAIAnimal::SetFullness()
{
	bIsHungry = !bIsHungry;
	if (!bIsHungry)
	{
		Fullness = 100.0f;
		UE_LOG(LogTemp, Warning, TEXT("SetFullness :: Fullness , IsHungry %f, %d"), Fullness, bIsHungry);
	}
	BlackboardComponent->SetValueAsBool("IsHungry", bIsHungry);
}

void ABaseAIAnimal::GenerateRandom()
{
	//int32 RandomPersonality = FMath::RandRange(0, static_cast<int32>(EAnimalAIPersonality::End) - 1);
	//Personality = static_cast<EAnimalAIPersonality>(RandomPersonality);
	Personality = EAnimalAIPersonality::Brave;
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
	UE_LOG(LogTemp, Warning, TEXT("DecreaseFullness :: Fullness , IsHungry %f, %d"), Fullness, bIsHungry);
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

void ABaseAIAnimal::OnHit(AActor* InstigatorActor)
{
	EMBER_LOG(LogTemp, Warning, TEXT("%s"), *InstigatorActor->GetName());
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
	return nullptr; //NavInvokerComponent;
}

void ABaseAIAnimal::SetDetails()
{
	switch (Personality)
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
