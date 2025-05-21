#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "NavigationInvokerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "EMSFunctionLibrary.h"
#include "MeleeTraceComponent.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/EmberHpBarUserWidget.h"
#include "UI/EmberWidgetComponent.h"

ABaseAIAnimal::ABaseAIAnimal()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bIsShouldSwim = false;
	CurrentState = EAnimalAIState::Idle;
	
	
	NavGenerationRadius = 4000.0f; //시각,청각 인지 버뮈보다 인보커 생성 범위가 커야함
	NavRemovalRadius = 4300.0f;
	NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>("NavInvokerComponent");

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	CharacterAttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
	AnimalAttributeSet = CreateDefaultSubobject<UEmberAnimalAttributeSet>(TEXT("AnimalAttributeSet"));

	FEmberAnimalAttributeData AttributeData;
	GenerateRandom();
	AttributeData.Fullness = Fullness;
	AttributeData.WalkSpeed = WalkSpeed;
	AttributeData.WanderRange = WanderRange;
	AttributeData.WildPower = WildPower;
	AnimalAttributeSet->InitFromData(AttributeData);

	MeleeTraceComponent = CreateDefaultSubobject<UMeleeTraceComponent>(TEXT("MeleeTraceComponent"));

	HpBarWidget = CreateDefaultSubobject<UEmberWidgetComponent>(TEXT("HpBarWidget"));
	HpBarWidget->SetupAttachment(GetMesh());
	HpBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

	SetHiddenInGame();
}

void ABaseAIAnimal::SetHiddenInGame()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
}

void ABaseAIAnimal::SetVisibleInGame()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void ABaseAIAnimal::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitStats(UEmberCharacterAttributeSet::StaticClass(), nullptr);
	AbilitySystemComponent->InitStats(UEmberAnimalAttributeSet::StaticClass(), nullptr);
}

void ABaseAIAnimal::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void ABaseAIAnimal::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = AnimalTagContainer;
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

	
	//InitAbilityActorInfo 호출 위치: 네트워크 플레이가 아니고 싱글 플레이나 로컬 전용이라면 괜찮음
	//서버와 클라이언트 동기화가 중요하다면 BeginPlay()에서 호출
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartAbilities)
		{
			if (AbilityClass) //Ability : TSubclassOf<UGameplayAbility> -> 어빌리티 상속받으면 다 ok 클래스에 대한 포인터 정보를 담은 템플릿
						//*Ability : 실제 클래스 포인터 (UClass*) 로 암묵적 변환됨 == AbilityClass.Get(), 이 타입의 클래스가 지정되어 있는가?
			{
				FGameplayAbilitySpec AbilitySpec(AbilityClass);
				AbilitySystemComponent->GiveAbility(AbilitySpec);
			}
		}
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberAnimalAttributeSet::GetFullnessAttribute()).
								AddUObject(this, &ThisClass::OnFullnessChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberCharacterAttributeSet::GetHealthAttribute()).
								AddUObject(this, &ThisClass::OnHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UEmberCharacterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);
		if (const UEmberCharacterAttributeSet* Attribute = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
		{
			const_cast<UEmberCharacterAttributeSet*>(Attribute)->OnHit.AddDynamic(this, &ABaseAIAnimal::OnHit);
		}
	}
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABaseAIAnimal::DecreaseFullness, 5.0f, true);

	PatrolPoints.SetNum(4);

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 800.0f; // AI가 다른 에이전트를 감지할 반경
	GetCharacterMovement()->AvoidanceWeight = 0.5f;
}

void ABaseAIAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("SetFullness :: Fullness , IsHungry %f, %d"), Fullness, bIsHungry);
}

void ABaseAIAnimal::OnHit(AActor* InstigatorActor)
{
	//현재 InstigatorActor = 플레이어스테이트 , 동물이 때린다면?
	//-> 만약 터지면 동물도 스테이트 쓸건지 체크하는 ai 설정 있음, 그걸 쓸건지 아니며 다른방법 찾던지, 일단 보류
	EMBER_LOG(LogTemp, Warning, TEXT("%s"), *InstigatorActor->GetName());
	//속도 빨라졌다 서서히 감소 추가해야함->이팩트로 적용예정
	
	if (BlackboardComponent)
	{
		BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Attacked"); 
		if (AActor* TargetActor = Cast<AController>(InstigatorActor->GetOwner())->GetPawn())
		{
			BlackboardComponent->SetValueAsObject("TargetActor", TargetActor);
		}
		if (Personality == EAnimalAIPersonality::Brave)
		{
			BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Attack"); 
		}
	}
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
	}
}

void ABaseAIAnimal::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT(" ABaseAIAnimal::OnHealthChanged::성공"));
}

void ABaseAIAnimal::OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	UE_LOG(LogTemp, Warning, TEXT("ABaseAIAnimal::OnMaxHealthChanged::성공"));
}

void ABaseAIAnimal::OnFullnessChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	//들어오면 무조건 배부름 상태
	Fullness = OnAttributeChangeData.NewValue;
	bIsHungry = false;
	BlackboardComponent->SetValueAsFloat("Fullness", Fullness);
	BlackboardComponent->SetValueAsBool("IsHungry", bIsHungry);
	BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Idle");
	BlackboardComponent->SetValueAsObject("NTargetFood", nullptr);
	BlackboardComponent->SetValueAsVector("NTargetFoodLocation", GetActorLocation());
}

void ABaseAIAnimal::GenerateRandom()
{
	int32 RandomPersonality = FMath::RandRange(0, static_cast<int32>(EAnimalAIPersonality::End) - 1);
	Personality = static_cast<EAnimalAIPersonality>(RandomPersonality);
	SetDetails();
	Fullness = FMath::FRandRange(50.f, 100.f);
	bIsHungry = Fullness <= 50.f;
}

void ABaseAIAnimal::DecreaseFullness()
{
	Fullness -= 1.f;
	Fullness = FMath::Clamp(Fullness, 0.0f, 100.0f);
	if (bIsHungry == false && Fullness <= 50.0f)
	{
		bIsHungry = true;
		BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Idle");
		BlackboardComponent->SetValueAsBool("IsHungry", bIsHungry);
	}
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

float ABaseAIAnimal::GetWanderRange() const
{
	return WanderRange;
}

const UAnimMontage* ABaseAIAnimal::GetMontage()
{
	return Montage;
}

TArray<FVector>& ABaseAIAnimal::GetPatrolPoints()
{
	return PatrolPoints;
}

FGameplayTagContainer& ABaseAIAnimal::GetGameplayTagContainer()
{
	return AnimalTagContainer;
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

void ABaseAIAnimal::PlayInteractMontage(uint8 InState)
{
	if (Montage)
	{
		PlayAnimMontage(Montage, 1.0f);
	}
}

EAnimalAIState ABaseAIAnimal::GetCurrentState()
{
	return CurrentState;
}

EAnimalAIPersonality ABaseAIAnimal::GetPersonality()
{
	return Personality;
}

void ABaseAIAnimal::SetDetails()
{
	switch (Personality)
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
		{
			WalkSpeed = 300.0f;
			WanderRange = 500.0f;
		}
		break;
	}
}