#include "ProjectEmber/AIAnimal/BaseAIAnimal.h"

#include "AIAnimalController.h"
#include "NavigationInvokerComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "MeleeTraceComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TokenRaidSubsystem.h"
#include "Attribute/Animal/EmberAnimalAttributeSet.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
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
	NavInvokerComponent->SetGenerationRadii(NavGenerationRadius, NavRemoveRadius);
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	CharacterAttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("CharacterAttributeSet"));
	AnimalAttributeSet = CreateDefaultSubobject<UEmberAnimalAttributeSet>(TEXT("AnimalAttributeSet"));
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
	
	GetCharacterMovement()->bUseRVOAvoidance = true;
	GetCharacterMovement()->AvoidanceConsiderationRadius = 800.0f; // AI가 다른 AI 감지할 반경
	GetCharacterMovement()->AvoidanceWeight = 0.5f;
	WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GenerateRandom();
	if (AnimalAttributeSet)
	{
		FEmberAnimalAttributeData AttributeData;
		AttributeData.Fullness = Fullness;
		AttributeData.WalkSpeed = WalkSpeed;
		AttributeData.WildPower = WildPower;
		AnimalAttributeSet->InitFromData(AttributeData);
	}
	
	AIController = Cast<AAIAnimalController>(GetController());
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

		AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &ABaseAIAnimal::OnAbilityEnd);
		

		if (const UEmberCharacterAttributeSet* Attribute = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
		{
			Attribute->OnOutOfHealth.AddDynamic(this, &ABaseAIAnimal::OnBeginDeath);
		}
		
		if (const UEmberCharacterAttributeSet* Attribute = AbilitySystemComponent->GetSet<UEmberCharacterAttributeSet>())
		{
			const_cast<UEmberCharacterAttributeSet*>(Attribute)->OnHit.AddDynamic(this, &ABaseAIAnimal::OnHit);
		}
	}
	//시간 받아오는 델리게이트 구독-> 이미 스폰된 애들이 밤에 활동/비활동 결정
	if (UGameplayEventSubsystem* EventSystem = UGameplayEventSubsystem::GetGameplayEvent(GetWorld()))
	{
		EventSystem->OnGameEvent.AddDynamic(this, &ABaseAIAnimal::OnGameTimeChanged);
	}
	
	GetWorldTimerManager().SetTimer(FullnessTimerHandle, this, &ABaseAIAnimal::DecreaseFullness, 5.0f, true);
	
	
	/* 메세지버스 사용 예시 */
	// 함수 바인딩
	MessageDelegateHandle = FMessageDelegate::CreateUObject(this, &ThisClass::ReceiveMessage);

	// FName으로 키값(메세지) 지정하고 델리게이트 전달
	UMessageBus::GetInstance()->Subscribe(TEXT("HideAnimal"), MessageDelegateHandle);
	UMessageBus::GetInstance()->Subscribe(TEXT("FixSpeed"), MessageDelegateHandle);
	// 호출할 곳에서 
}

void ABaseAIAnimal::OnAbilityEnd(const FAbilityEndedData& AbilityEndedData)
{
	UGameplayAbility* EndedAbility = AbilityEndedData.AbilityThatEnded.Get();
	if (EndedAbility->IsA(StartAbilities[0])) // 토큰 공격이 끝나면
	{
		BlackboardComponent->SetValueAsBool("IsAbility", false);
		if (bHasToken)
		{
			FVector BestLocation = GetGameInstance()->GetSubsystem<UTokenRaidSubsystem>()->GetBestLocation(*this);
			BlackboardComponent->SetValueAsVector("SafeLocation", BestLocation);
		}
	}
}

void ABaseAIAnimal::OnBeginDeath()
{
	bIsDead = true;
	bIsShouldSleep = false;
	for (UActorComponent* Component : GetComponents())
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
			{
				PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
	}
	/** EmberChararcter의 TargetSystem을 위한 브로드 캐스트 및 본인에게 달려있는 타겟위젯 지우기 */
	UMessageBus::GetInstance()->BroadcastMessage(TEXT("DeathEnemy"), this);
	TArray<UWidgetComponent*> WidgetComps;
	GetComponents<UWidgetComponent>(WidgetComps);
	for (UWidgetComponent* Comp : WidgetComps)
	{
		if (Comp && Comp != HpBarWidget)
		{
			Comp->DestroyComponent();
		}
	}
	
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
		AIController->BrainComponent->StopLogic(TEXT("HiddenInGame"));
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
			if (AnimalAttributeSet) 
			{
				AnimalAttributeSet->SetWalkSpeed(WalkSpeed);
			}
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
	}
	
	if (CharacterAttributeSet)
	{
		CharacterAttributeSet->SetHealth(CharacterAttributeSet->GetMaxHealth());
		FOnAttributeChangeData ChangeData;
		ChangeData.NewValue = CharacterAttributeSet->GetHealth();
		//Cast<UEmberHpBarUserWidget>(HpBarWidget->GetWidget())->OnHealthChanged(ChangeData);
	}
	
	for (UActorComponent* Component : GetComponents())
	{
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Component))
		{
			PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	}
	
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
		if (bIsShouldSwim)
		{
			ApplyWaterSurface(DeltaTime);
		}
		else
		{
			SwimTime = 0.f;
		}
	}
	if (BlackboardComponent)
	{
		if (FName("Animal.Montage.Attack") == BlackboardComponent->GetValueAsName("NStateTag"))
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
}

void ABaseAIAnimal::TriggerSpeedUp()
{
	FGameplayEventData Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag("Trigger.Animal.SpeedUp");
	Payload.Instigator = this;
	Payload.EventMagnitude = WalkSpeed;
	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
}

void ABaseAIAnimal::OnHit(AActor* InstigatorActor)
{
	//도망가는 상황에서만 속도 빨라졌다 감소 추가해야함->이팩트로 적용예정
	bIsShouldSleep = false;
	HitCount++;
	TriggerSpeedUp();
	
	if (BlackboardComponent)
	{
		if (AActor* TargetActor = Cast<AController>(InstigatorActor->GetOwner())->GetPawn())
		{
			//스폐셜공격 없으면 리턴
			if (!MontageMap.Contains(FGameplayTag::RequestGameplayTag("Animal.Montage.AttackSpecial")))
			{
				return;
			}
			//뒤에서 맞으면 스폐셜공격
			FVector Direction = TargetActor->GetActorLocation() - GetActorLocation();
			Direction.Normalize();
			FVector ForwardVector = GetActorForwardVector();
			float Dot = FVector::DotProduct(ForwardVector, Direction);
			if (Dot<0)//뒤쪽이라면
			{
				OnAttackSpecial();
				return;
			}
			BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Attacked"); 
		}
		if (Personality == EAnimalAIPersonality::Brave)
		{
			//맞았고 성격 == 용감이면 무조건 반격
			BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Attack"); 
		}
	}
}

void ABaseAIAnimal::OnAttackSpecial()
{
	BlackboardComponent->SetValueAsBool("IsAbility", true);

	FGameplayEventData Payload;
	Payload.EventTag = FGameplayTag::RequestGameplayTag("Trigger.Animal.AttackSpecial");
	Payload.Instigator = this;
	Payload.OptionalObject = MontageMap[FGameplayTag::RequestGameplayTag("Animal.Montage.AttackSpecial")];
	AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
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
		BlackboardComponent->SetValueAsObject("NTargetFood", nullptr);
		BlackboardComponent->SetValueAsVector("NTargetFoodLocation", GetActorLocation());
	}
}

void ABaseAIAnimal::GenerateRandom()
{
	int32 RandomPersonality = FMath::RandRange(0, static_cast<int32>(EAnimalAIPersonality::End) - 1);
	Personality = static_cast<EAnimalAIPersonality>(RandomPersonality);
	Fullness = FMath::FRandRange(70.f, 90.f);
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

bool ABaseAIAnimal::IsTargetable_Implementation() const
{
	//ITargetSystemTargetableInterface::IsTargetable_Implementation();
	return !bIsDead;
}


float ABaseAIAnimal::GetWildPower() const
{
	return WildPower;
}

TObjectPtr<UAnimMontage> ABaseAIAnimal::GetMontage(FGameplayTag MontageTag)
{
	if (MontageTag == FGameplayTag::RequestGameplayTag("Animal.Montage.Attack"))
	{
		int32 NewIndex = FMath::RandRange(0,AttackMontages.Num()-1);
		MontageMap[MontageTag] = AttackMontages[NewIndex];
	}
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
		{ //올라가는 방향, 스피드 더 빠르게, 크기도 전체적으로 감싸는 느낌으로, 차라리 밑에 오오라 느김도 괜찮을 듯, 컬러 스테일이랑 다이나믹 머테리얼 파라미터 커브값이 0으로 되면서 안보인 듯
			NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		   NiagaraSystem,
		   GetMesh(),
		   NAME_None, // 없으면 NAME_None
		   FVector::ZeroVector,
		   FRotator::ZeroRotator,
		   EAttachLocation::SnapToTarget,
		   false,  // bAutoDestroy
		   true   // bAutoActivate
		   );
		}
	}
}

void ABaseAIAnimal::SetState(bool IsShouldSleep)
{
	if (!BlackboardComponent)
	{
		return;
	}
	
	// 생성, 리스폰 때 활동/비활동인지
	if (IsShouldSleep)
	{
		DeactiveSleep();
	}
	else
	{
		ActiveNonSleep();
	}
}

void ABaseAIAnimal::OnGameTimeChanged(const FGameplayTag& EventTag, const FGameplayEventData& EventData) 
{
	if (bIsDead)
	{
		return;
	}
	//월드에 있을 때 밤->낮 바뀔 때
	if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Time.Day"))))
	{
		// 활동 시작
		ActiveNonSleep();
	}
	else if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Time.Night"))))
	{
		FName State = BlackboardComponent->GetValueAsName("NStateTag");
		FGameplayTag StateTag = FGameplayTag::RequestGameplayTag(State);
		if (StateTag == FGameplayTag::RequestGameplayTag("Animal.State.Attack")||
			StateTag == FGameplayTag::RequestGameplayTag("Animal.State.Attacked")||bIsShouldSwim)
		{
			return;
		}
		
		// 밤에 활동 확률 설정
		MakeRandomActiveAtNight(EventData.EventMagnitude); // 0.맑음, 1.흐린 2.비 3.천둥
	}
}

void ABaseAIAnimal::MakeRandomActiveAtNight(int32 InWeather)
{
	//--- 밤에 활동 확률 설정 --------------------------------------------------
	int32 WeatherCopy = InWeather;
	float AttackProb = 0.15;          // 기본 15 %
	
	// 날씨가 나쁠수록  0.25% =>  0, 0.25, 0.5, 0.75
	WeatherCopy *= 0.25f;
	AttackProb += WeatherCopy;           // 총합 ⇒ 0.15, 0.4, 0.65, 0.9
	//--------------------------------------------------------------------

	// 난수 뽑아서 결정
	if (FMath::FRand() <= AttackProb)
	{
		ActiveNonSleep();
	}
	else
	{
		DeactiveSleep();
	}
}

void ABaseAIAnimal::ActiveNonSleep() 
{
	bIsShouldSleep = false;
	BlackboardComponent->SetValueAsBool("IsSleep",false);
	BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Idle"); 
}

void ABaseAIAnimal::DeactiveSleep() 
{
	//잠드는 것만 잘 곳 먼저 찾고 bIsShouldSleep 변경으로 애니메이션 재생
	BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Sleeping"); 
}


void ABaseAIAnimal::ApplyWaterSurface(float DeltaTime) 
{
	APhysicsVolume* PhysicsVolume = GetCharacterMovement()->GetPhysicsVolume();
	FVector VolumeOrigin;
	FVector VolumeExtent;
	PhysicsVolume->GetActorBounds(false, VolumeOrigin, VolumeExtent);
	
	
	WaterSurfaceZ = VolumeOrigin.Z;
	SwimTime += DeltaTime;
	float WaveOffsetZ = FMath::Sin(SwimTime * FloatFrequency * PI) * FloatAmplitude;

	FVector NewLocation = GetActorLocation();
	NewLocation.Z = WaterSurfaceZ + WaveOffsetZ;
	SetActorLocation(NewLocation);
	DrawDebugBox(GetWorld(), VolumeOrigin, VolumeExtent, FColor::Blue, false, 2.0f);
}


bool ABaseAIAnimal::GetIsShouldSleep() const
{
	return bIsShouldSleep;
}

void ABaseAIAnimal::SetIsShouldSleep(bool InIsSleep)
{
	bIsShouldSleep = InIsSleep;
}

int32 ABaseAIAnimal::GetHitCount() const
{
	return HitCount;
}

void ABaseAIAnimal::SetHitCount(int32 InHitCount)
{
	HitCount = InHitCount;
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

void ABaseAIAnimal::SetHasToken(const bool InHasToken)
{
	bHasToken = InHasToken;
	BlackboardComponent->SetValueAsBool("HasToken",bHasToken);
}

bool ABaseAIAnimal::GetHasToken() const
{
	return bHasToken;
}

void ABaseAIAnimal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 타이머 해제
	GetWorldTimerManager().ClearTimer(FullnessTimerHandle);

	if (UGameplayEventSubsystem* EventSystem = UGameplayEventSubsystem::GetGameplayEvent(GetWorld()))
	{
		EventSystem->OnGameEvent.RemoveDynamic(this, &ABaseAIAnimal::OnGameTimeChanged);
	}
	
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

void ABaseAIAnimal::SwitchBehaviorTree()
{
	Cast<AAIAnimalController>(GetController())->SwitchToBehaviorTree(1);
	BlackboardComponent->SetValueAsName("NStateTag", "Animal.State.Idle");
}

