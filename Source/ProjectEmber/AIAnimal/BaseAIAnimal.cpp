#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "NavigationInvokerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "EMSFunctionLibrary.h"
#include "MeleeTraceComponent.h"
#include "AI/NavigationSystemBase.h"
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
	NavGenerationRadius = 4000.0f; //시각,청각 인지 버뮈보다 인보커 생성 범위가 커야함
	NavRemoveRadius = 4300.0f;
	NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>("NavInvokerComponent");
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	CharacterAttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
	AnimalAttributeSet = CreateDefaultSubobject<UEmberAnimalAttributeSet>(TEXT("AnimalAttributeSet"));

	GenerateRandom();
	FEmberAnimalAttributeData AttributeData;
	AttributeData.Fullness = Fullness;
	AttributeData.WalkSpeed = WalkSpeed;
	AttributeData.WanderRange = WanderRange;
	AttributeData.WildPower = WildPower;
	AnimalAttributeSet->InitFromData(AttributeData);

	MeleeTraceComponent = CreateDefaultSubobject<UMeleeTraceComponent>(TEXT("MeleeTraceComponent"));

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
	NavInvokerComponent->SetGenerationRadii(NavGenerationRadius, NavRemoveRadius);
	
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

		HpBarWidget->UpdateAbilitySystemComponent(this);
	}
	
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

		if (const UEmberCharacterAttributeSet* Attribute = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
		{
			Attribute->OnOutOfHealth.AddDynamic(this, &ABaseAIAnimal::OnBeginDeath);
		}
		
		if (const UEmberCharacterAttributeSet* Attribute = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
		{
			const_cast<UEmberCharacterAttributeSet*>(Attribute)->OnHit.AddDynamic(this, &ABaseAIAnimal::OnHit);
		}
	}
	
	GetWorldTimerManager().SetTimer(FullnessTimerHandle, this, &ABaseAIAnimal::DecreaseFullness, 5.0f, true);
	//PatrolPoints.SetNum(4);

	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 400.0f; // AI가 다른 에이전트를 감지할 반경
	GetCharacterMovement()->AvoidanceWeight = 0.5f;

	/* 메세지버스 사용 예시 */
	// 함수 바인딩
	MessageDelegateHandle = FMessageDelegate::CreateUObject(this, &ThisClass::ReceiveMessage);

	// FName으로 키값(메세지) 지정하고 델리게이트 전달
	UMessageBus::GetInstance()->Subscribe(TEXT("HideAnimal"), MessageDelegateHandle);

	// 호출할 곳에서 
	
	
}

void ABaseAIAnimal::OnBeginDeath()
{
	bIsDead = true;
	
	SetActorTickEnabled(false);
	if (AIController && AIController->BrainComponent && BlackboardComponent)
	{
		BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Death"); 
		AIController->BrainComponent->Cleanup();
		AIController->BrainComponent->StopLogic(TEXT("HiddenInGame"));
	}
	
	FGameplayEventData Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag("Trigger.Animal.Death");
	Payload.Instigator = this;
	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
	
}


void ABaseAIAnimal::ReceiveMessage(const FName MessageType, UObject* Payload)
{
	//파밍시간 종료 -> 어빌리티에서 메세지버스로 호출될 함수 -> 완전 죽음으로 숨김처리
	if (TEXT("HideAnimal") == MessageType)
	{
		if (Payload == this)
		{
			SetHiddenInGame();
		}
	}
}


void ABaseAIAnimal::SetHiddenInGame()
{
	UE_LOG(LogTemp, Warning, TEXT("[SetHiddenInGame] Hiding %s (%p)"), *GetName(), this);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	 SetActorTickEnabled(false);
	 if (AIController && AIController->BrainComponent && BlackboardComponent)
	 {
	 	BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Death"); 
	 	AIController->BrainComponent->Cleanup();
	 	AIController->BrainComponent->StopLogic(TEXT("HiddenInGame")); //스폰시 숨김처리
	 }
}

void ABaseAIAnimal::SetVisibleInGame()
{
	if (CharacterAttributeSet)
	{
		CharacterAttributeSet->SetHealth(CharacterAttributeSet->GetMaxHealth());
		FOnAttributeChangeData ChangeData;
		ChangeData.NewValue = CharacterAttributeSet->GetHealth();
		Cast<UEmberHpBarUserWidget>(HpBarWidget->GetWidget())->OnHealthChanged(ChangeData);
	}
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	if (AIController && AIController->BrainComponent && BlackboardComponent)
	{
		BlackboardComponent = AIController->GetBlackboardComponent();
		BlackboardComponent->ClearValue("TargetObject");
		BlackboardComponent->SetValueAsName("NStateTag","Animal.State.Idle");
		AIController->BrainComponent->StartLogic();
	}
}

void ABaseAIAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ABaseAIAnimal::OnHit(AActor* InstigatorActor)
{
	//도망가는 상황에서만 속도 빨라졌다 서서히 감소 추가해야함->이팩트로 적용예정
	
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

void ABaseAIAnimal::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	Cast<UEmberHpBarUserWidget>(HpBarWidget->GetWidget())->OnHealthChanged(OnAttributeChangeData);
}

void ABaseAIAnimal::OnFullnessChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	//감소하거나 증가하거나
	Fullness = OnAttributeChangeData.NewValue;
	
	Fullness = FMath::Clamp(Fullness, 0.0f, 100.0f);
	
	
	if (bIsHungry == false && Fullness <= 50.0f)
	{
		bIsHungry = true;
		BlackboardComponent->SetValueAsBool("IsHungry", bIsHungry);
	}
	else
	{
		bIsHungry = false;
		BlackboardComponent->SetValueAsBool("IsHungry", bIsHungry);
		BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Idle");
		BlackboardComponent->SetValueAsObject("NTargetFood", nullptr);
		BlackboardComponent->SetValueAsVector("NTargetFoodLocation", GetActorLocation());
	}
}

void ABaseAIAnimal::GenerateRandom()
{
	//int32 RandomPersonality = FMath::RandRange(0, static_cast<int32>(EAnimalAIPersonality::End) - 1);
	int32 RandomPersonality =3; //임시수정
	Personality = static_cast<EAnimalAIPersonality>(RandomPersonality);
	SetDetails();
	Fullness = FMath::FRandRange(30.f, 70.f);
	bIsHungry = Fullness <= 50.f;
}

void ABaseAIAnimal::DecreaseFullness()
{
	FGameplayEventData Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag("Trigger.Animal.Decrease");
	Payload.Instigator = this;
	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
}


 void ABaseAIAnimal::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
 {
 	TagContainer;
 }

float ABaseAIAnimal::GetWildPower() const
{
	return WildPower;
}

float ABaseAIAnimal::GetWanderRange() const //아무데서도 안 쓰임
{
	return WanderRange;
}

UAnimMontage* ABaseAIAnimal::GetMontage(FGameplayTag MontageTag)
{
	return MontageMap[MontageTag];
}

TArray<FVector>& ABaseAIAnimal::GetPatrolPoints()
{
	return PatrolPoints;
}

FGameplayTag ABaseAIAnimal::GetIdentityTag() const
{
	return IdentityTag;
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

void ABaseAIAnimal::SetIdentityTag(const FGameplayTag InIdentityTag)
{
	IdentityTag= InIdentityTag;
}

FName ABaseAIAnimal::GetRoleTag() const
{
	if (!BlackboardComponent)
	{
		return "None"; 
	}
	return BlackboardComponent->GetValueAsName("NGroupTag");
}


void ABaseAIAnimal::SetRoleTag(const FName InRoleTag)
{
	if (!BlackboardComponent)
	{
		return; 
	}
	BlackboardComponent->SetValueAsName("NGroupTag", InRoleTag);
}

void ABaseAIAnimal::SetIdleState()
{
	if (!BlackboardComponent)
	{
		return; 
	}
	BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Idle");
}

bool ABaseAIAnimal::GetIsDead() const
{
	return bIsDead;
}

void ABaseAIAnimal::SetIsDead(const bool InIsDead)
{
	bIsDead = InIsDead;
}

float ABaseAIAnimal::GetSoundPitch() const
{
	return SoundPitch;
}

void ABaseAIAnimal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 타이머 해제
	GetWorldTimerManager().ClearTimer(FullnessTimerHandle);
	
	UMessageBus::GetInstance()->Unsubscribe(TEXT("HideAnimal"), MessageDelegateHandle);
	
	Super::EndPlay(EndPlayReason);
}