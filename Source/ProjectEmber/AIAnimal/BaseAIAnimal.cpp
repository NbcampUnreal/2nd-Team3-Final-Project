#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "NavigationInvokerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "EMSFunctionLibrary.h"
#include "MeleeTraceComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/NavigationSystemBase.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/EmberHpBarUserWidget.h"
#include "UI/EmberWidgetComponent.h"
#include "Quest/QuestSubsystem.h"

ABaseAIAnimal::ABaseAIAnimal()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bIsShouldSwim = false;
	NavGenerationRadius = 4000.0f; //시각,청각 인지 버뮈보다 인보커 생성 범위가 커야함
	NavRemoveRadius = 4300.0f;
	NavInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavInvokerComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	CharacterAttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
	AnimalAttributeSet = CreateDefaultSubobject<UEmberAnimalAttributeSet>(TEXT("AnimalAttributeSet"));
	RCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RCapsuleComponent"));
	RCapsuleComponent->SetCapsuleHalfHeight(88.0f);
	RCapsuleComponent->SetCapsuleRadius(44.0f);
	LCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LCapsuleComponent"));
	LCapsuleComponent->SetCapsuleHalfHeight(88.0f);
	LCapsuleComponent->SetCapsuleRadius(44.0f);
	
	GenerateRandom();
	FEmberAnimalAttributeData AttributeData;
	AttributeData.Fullness = Fullness;
	AttributeData.WalkSpeed = WalkSpeed;
	AttributeData.WanderRange = WanderRange;
	AttributeData.WildPower = WildPower;
	AnimalAttributeSet->InitFromData(AttributeData);

	MeleeTraceComponent = CreateDefaultSubobject<UMeleeTraceComponent>(TEXT("MeleeTraceComponent"));

	// HpBarWidget = CreateDefaultSubobject<UEmberWidgetComponent>(TEXT("HpBarWidget"));
	// HpBarWidget->SetupAttachment(GetMesh());
	// HpBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));
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
	
	// if (HpBarWidgetClass)
	// {
	// 	HpBarWidget->SetWidgetClass(HpBarWidgetClass);
	// 	HpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	// 	HpBarWidget->SetDrawSize(FVector2D(200.0f, 20.0f));
	// 	HpBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//
	// 	HpBarWidget->UpdateAbilitySystemComponent(this);
	// }
	
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
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UEmberAnimalAttributeSet::GetWalkSpeedAttribute()).
										AddUObject(this, &ThisClass::OnWalkSpeedChanged);
		

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
	GetCharacterMovement()->AvoidanceConsiderationRadius = 800.0f; // AI가 다른 AI 감지할 반경
	GetCharacterMovement()->AvoidanceWeight = 0.5f;
	
	/* 메세지버스 사용 예시 */
	// 함수 바인딩
	MessageDelegateHandle = FMessageDelegate::CreateUObject(this, &ThisClass::ReceiveMessage);

	// FName으로 키값(메세지) 지정하고 델리게이트 전달
	UMessageBus::GetInstance()->Subscribe(TEXT("HideAnimal"), MessageDelegateHandle);
	UMessageBus::GetInstance()->Subscribe(TEXT("FixSpeed"), MessageDelegateHandle);
	// 호출할 곳에서 
}

void ABaseAIAnimal::OnBeginDeath()
{
	bIsDead = true;
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	GetWorldTimerManager().PauseTimer(FullnessTimerHandle);
	if (NiagaraComponent)
	{
		NiagaraComponent->Deactivate();
	}
	if (AIController && AIController->BrainComponent && BlackboardComponent)
	{
		BlackboardComponent->SetValueAsObject("TargetActor",nullptr); 
		BlackboardComponent->SetValueAsObject("NTargetFood",nullptr); 
		BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Death"); 
		AIController->BrainComponent->Cleanup();
		AIController->BrainComponent->StopLogic(TEXT("HiddenInGame")); //스폰시 숨김처리
	}

	if (UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		FGameplayTag EventTag = FGameplayTag::RequestGameplayTag("Quest.Animal.Kill");
		FGameplayEventData Data;
		Data.EventTag = EventTag;
		Data.Target = this;
		Data.TargetTags.AddTag(IdentityTag); // 예: Animal.Boar 또는 Animal.Wolf
		QuestSubsystem->OnGameEvent(EventTag, Data);
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

	if (TEXT("FixSpeed") == MessageType)
	{
		if (Payload == this)
		{
			AnimalAttributeSet->SetWalkSpeed(WalkSpeed);
		}
	}
}

void ABaseAIAnimal::SetHiddenInGame()
{
	SetActorHiddenInGame(true);
}

void ABaseAIAnimal::SetVisibleInGame()
{
	bIsDead = false;
	if (NiagaraComponent)
	{
		NiagaraComponent->Activate(true);
	}
	
	if (AIController && AIController->BrainComponent)
	{
		AIController->BrainComponent->StartLogic();
		BlackboardComponent = AIController->GetBlackboardComponent();
		BlackboardComponent->ClearValue("TargetObject");
		BlackboardComponent->SetValueAsName("NStateTag","Animal.State.Idle");
		
	}
	
	if (CharacterAttributeSet)
	{
		CharacterAttributeSet->SetHealth(CharacterAttributeSet->GetMaxHealth());
		FOnAttributeChangeData ChangeData;
		ChangeData.NewValue = CharacterAttributeSet->GetHealth();
		//Cast<UEmberHpBarUserWidget>(HpBarWidget->GetWidget())->OnHealthChanged(ChangeData);
	}
	
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	GetWorldTimerManager().UnPauseTimer(FullnessTimerHandle);
	SetActorHiddenInGame(false);
}

void ABaseAIAnimal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetMovementComponent())
	{
		bIsShouldSwim = GetMovementComponent()->IsSwimming();
	}
	if (BlackboardComponent)
	{
		UObject* TargetObject = BlackboardComponent->GetValueAsObject("TargetActor");
		if (AActor* Target = Cast<AActor>(TargetObject))
		{
			FVector Direction = (Target->GetActorLocation()- GetActorLocation()).GetSafeNormal2D(); //z무시
			FRotator TargetRotation = Direction.Rotation(); //얼마나 회전해야하는지
			FRotator NewRot = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
			SetActorRotation(NewRot);
		}
	}
}

void ABaseAIAnimal::OnHit(AActor* InstigatorActor)
{
	//도망가는 상황에서만 속도 빨라졌다 감소 추가해야함->이팩트로 적용예정

	FGameplayEventData Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag("Trigger.Animal.SpeedUp");
	Payload.Instigator = this;
	Payload.EventMagnitude = WalkSpeed;
	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
	
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

void ABaseAIAnimal::OnWalkSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	GetCharacterMovement()->MaxWalkSpeed = OnAttributeChangeData.NewValue;
}

void ABaseAIAnimal::OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	//Cast<UEmberHpBarUserWidget>(HpBarWidget->GetWidget())->OnHealthChanged(OnAttributeChangeData);
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
	else if (bIsHungry && Fullness >= 90.0f)
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
	int32 RandomPersonality = FMath::RandRange(0, static_cast<int32>(EAnimalAIPersonality::End) - 1);
	//int32 RandomPersonality =3; //임시수정
	Personality = static_cast<EAnimalAIPersonality>(RandomPersonality);
	SetDetails();
	Fullness = FMath::FRandRange(30.f, 40.f);
}

void ABaseAIAnimal::DecreaseFullness()
{
	if (Fullness <= 0.f)
	{
		return;
	}
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
	RoleTag = FGameplayTag::RequestGameplayTag(InRoleTag);
	BlackboardComponent->SetValueAsName("NGroupTag", InRoleTag);

		
	if (RoleTag == FGameplayTag::RequestGameplayTag("Animal.Role.Leader"))
	{
		FVector NewScale = GetActorScale3D()*1.5f;
		SetActorScale3D(NewScale);
		GetMesh()->SetMaterial(0,MaterialInterface[1]);
	
		UNiagaraSystem* NiagaraSystem = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Game/SoStylized/Effects/Systems/NS_ElectricBuildup.NS_ElectricBuildup"));
		if (NiagaraSystem)
		{
			NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		   NiagaraSystem,
		   GetMesh(),
		   TEXT("spine_04"), // 없으면 NAME_None
		   FVector::ZeroVector,
		   FRotator::ZeroRotator,
		   EAttachLocation::SnapToTarget,
		   false,  // bAutoDestroy
		   true   // bAutoActivate
		   );
		}
	}
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

int32 ABaseAIAnimal::GetSoundIndex() const
{
	return SoundIndex;
}

void ABaseAIAnimal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 타이머 해제
	GetWorldTimerManager().ClearTimer(FullnessTimerHandle);
	
	UMessageBus::GetInstance()->Unsubscribe(TEXT("HideAnimal"), MessageDelegateHandle);
	UMessageBus::GetInstance()->Unsubscribe(TEXT("FixSpeed"), MessageDelegateHandle);
	if (NiagaraComponent)
	{
		NiagaraComponent->Deactivate();         
		NiagaraComponent->DestroyComponent(); 
		NiagaraComponent = nullptr;
	}
	
	Super::EndPlay(EndPlayReason);
}