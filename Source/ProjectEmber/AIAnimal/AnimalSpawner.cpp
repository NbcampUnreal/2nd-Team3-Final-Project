

#include "AnimalSpawner.h"

#include "AIController.h"
#include "AnimalSpawnPoint.h"
#include "BaseAIAnimal.h"
#include "TokenRaidSubsystem.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "GameInstance/GameplayEventSubsystem.h"


AAnimalSpawner::AAnimalSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAnimalSpawner::BeginPlay()
{
	Super::BeginPlay();

	//타이머 1초마다 플레이어와 거리 체크
	GetWorldTimerManager().SetTimer(DistanceTimerHandle, this, &AAnimalSpawner::DistanceCheck, 1.0f, true);

	//GetWorld()->GetTimerManager().PauseTimer(DistanceTimerHandle);
	//스포너 엑티베이트 함수 따로 만들면 들어갈 애들
	{
		//GetWorld()->GetTimerManager().UnPauseTimer(DistanceTimerHandle);
	// 함수 바인딩
	MessageDelegateHandle = FMessageDelegate::CreateUObject(this, &ThisClass::ReceiveMessage);

	// FName으로 키값(메세지) 지정하고 델리게이트 전달, 구독했으면 EndPlay에서 해제까지 꼭 하기
	UMessageBus::GetInstance()->Subscribe(TEXT("HideAnimal"), MessageDelegateHandle);
	}
	
	//시간 받아오는 델리게이트 구독->저장해뒀다가 생성/스폰 시킬 때 사용
	if (UGameplayEventSubsystem* EventSystem = UGameplayEventSubsystem::GetGameplayEvent(GetWorld()))
	{
		EventSystem->OnGameEvent.AddDynamic(this, &AAnimalSpawner::OnGameTimeChanged);
	}
}

void AAnimalSpawner::ReceiveMessage(const FName MessageType, UObject* Payload)
{
	//찐 죽었을 때
	if (TEXT("HideAnimal") == MessageType)
	{
		MessageMoveToDead(Payload);
	}
}

void AAnimalSpawner::MessageMoveToDead(UObject* Payload)
{
	if (ABaseAIAnimal* Animal = Cast<ABaseAIAnimal>(Payload))
	{
		for (FAnimalSpawnInfo& Info : AnimalsInfo)
		{
			if (Info.SpawnAnimals.Contains(Animal))
			{
				Info.SpawnAnimals.Remove(Animal);
				Info.DeadAnimals.Add(Animal);
				return;
			}
		}
	}
}

void AAnimalSpawner::OnGameTimeChanged(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
	if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Time.Day"))))
	{
		// 활동 시작
		bIsDay = true;
	}
	else if (EventTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Gameplay.Time.Night"))))
	{
		// 휴식 상태로 전환
		bIsDay = false;
		Weather = EventData.EventMagnitude; // 0.맑음, 1.흐린 2.비 3.천둥
	}
}

void AAnimalSpawner::MakeRandomActiveAtNight()
{
	//--- 밤에 활동 확률 설정 --------------------------------------------------
	int32 WeatherCopy = Weather;
	float AttackProb = 0.15;          // 기본 15 %
	
	 // 날씨가 나쁠수록  0.25% =>  0, 0.25, 0.5, 0.75
	WeatherCopy *= 0.25f;
	AttackProb += WeatherCopy;           // 총합 ⇒ 0.15, 0.4, 0.65, 0.9
	//--------------------------------------------------------------------

	// 난수 뽑아서 결정
	if (FMath::FRand() <= AttackProb)
	{
		bIsShouldSleep = false;
	}
	else
	{
		bIsShouldSleep = true;
	}
}

void AAnimalSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CreateInfoQueueByToken.IsEmpty())
	{
		TickCreateQueueByToken(CreateInfoQueueByToken);
	}
	if (!LoadInfoQueue.IsEmpty())
	{
		TickCreateQueue(LoadInfoQueue, bIsLoading);
	}
	else
	{
		TickCreateQueue(CreateInfoQueue, bIsLoading);
	}
	TickDespawnQueue();
	TickSpawnQueue();
}

void AAnimalSpawner::ActorPreSave_Implementation()
{
	IEMSActorSaveInterface::ActorPreSave_Implementation();
	SaveInfoArray.Empty();
	
	for (int32 i =0; i<AnimalsInfo.Num(); i++)
	{
		int32 SaveIndex = 0;
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : AnimalsInfo[i].SpawnAnimals)
		{
			if (Animal.IsValid() && !Animal->IsHidden() && !Animal->GetIsDead())
			{
				FName Tag = Animal->GetIdentityTag().GetTagName();
				FString SaveId =
					FString::Printf(TEXT("%s_Info%d_Per%d"), *Tag.ToString(),i, SaveIndex);

				TArray<TSoftObjectPtr<AAnimalSpawnPoint>> OutSpawnPoints = SpawnPoints; //원본은 수정되면 안됨
				float SpawnPointMinimum = 10000000.f;
				int32 Closest = -1;
				//포인트들 순회 -> 가장 가까우 포인트만 스폰 지역에서 제외
				for (int j=0; j < OutSpawnPoints.Num(); j++)
				{
					if (OutSpawnPoints[j].IsValid())
					{
						float Dist = FVector::Dist(OutSpawnPoints[j]->GetActorLocation(),Animal->GetActorLocation());
						if (Dist < SpawnPointMinimum)
						{
							SpawnPointMinimum = Dist;
							Closest = j;
						}
					}
				}
				
				FAnimalQueueInfo SaveInfo;
				SaveInfo.AnimalClass = AnimalsInfo[i].AnimalClass;
				SaveInfo.InitInfo.Location = Animal.Get()->GetActorLocation();
				SaveInfo.InitInfo.Rotation = Animal.Get()->GetActorRotation();
				SaveInfo.RoleTag = Animal->GetRoleTag();
				SaveInfo.SpawnInfoIndex = i;
				SaveInfo.SpawnPointIndex = Closest;
				
				SaveInfoArray.Emplace(SaveInfo);
				SaveIndex++;
			}
		}
	}
}

void AAnimalSpawner::ActorLoaded_Implementation()
{
	IEMSActorSaveInterface::ActorLoaded_Implementation();

	bIsLoading = true; 
	TryReleaseEntire();
	
	if (!SaveInfoArray.IsEmpty())
	{
		for (int32 i =0; i<SaveInfoArray.Num(); i++)
		{
			LoadInfoQueue.Enqueue(SaveInfoArray[i]);
		}
	}
}

void AAnimalSpawner::DistanceCheck()
{
	// 로드 중일 때는 새로 생성하지 않음
	if (bIsLoading)
	{
		return;
	}
	
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player)
	{
		return;
	}
	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if (SpawnDistance >= Distance)
	{
		if (!LoadInfoQueue.IsEmpty())
		{
			return;
		}
 		TArray<TSoftObjectPtr<AAnimalSpawnPoint>> OutSpawnPoints = DiscardNearestPoint();
		TArray<TSoftObjectPtr<AAnimalSpawnPoint>> BestSpawnPoints = SelectNearPoints(OutSpawnPoints);
		TryCreateQueue(BestSpawnPoints);
	}
	else if (ReleaseDistance <= Distance)
	{
		TryReleaseEntire();
	}
	else if (DespawnDistance <= Distance)
	{
		SortFarthestAnimal();
	}
	else // 스폰과 디스폰 사이
	{
	}
}

TArray<TSoftObjectPtr<AAnimalSpawnPoint>> AAnimalSpawner::DiscardNearestPoint()
{
	// 스폰 조건에 맞는 포인트에 생성
	int32 Closest = -1;
	float SpawnPointMinimum = 10000000.f;
	TArray<TSoftObjectPtr<AAnimalSpawnPoint>> OutSpawnPoints = SpawnPoints; //원본은 수정되면 안됨
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	

	//포인트들 순회 -> 가장 가까운 포인트만 스폰 지역에서 제외
	for (int i=0; i < OutSpawnPoints.Num(); i++)
	{
		if (OutSpawnPoints[i].IsValid())
		{
			float Dist = FVector::Dist(OutSpawnPoints[i]->GetActorLocation(),Player->GetActorLocation());
			if (Dist < SpawnPointMinimum)
			{
				SpawnPointMinimum = Dist;
				Closest = i;
			}
		}
	}
	OutSpawnPoints.RemoveAt(Closest);
	return OutSpawnPoints;
}

TArray<TSoftObjectPtr<AAnimalSpawnPoint>> AAnimalSpawner::SelectNearPoints(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints)
{
	//여기에 DiscardNearestPoint의 OutSpawnPoints 받아서 설정한 몇개의 포인트들만 다시 고르는 함수
	//가장 가까운 포인트가 제외된 InSpawnPoints,
	//InSpawnPoints중에서 플레이어와 가까운 순서로 정렬
	//PerSpawnPoints의 수만큼 정렬 순서대로 스폰
	//InSpawnPoints.Num() < PerSpawnPoints 라면 PerSpawnPoints = InSpawnPoints.Num() 으로 변경

	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	InSpawnPoints.Sort([Player](TSoftObjectPtr<AAnimalSpawnPoint> A, TSoftObjectPtr<AAnimalSpawnPoint> B)
	{
		float DistA = FVector::DistSquared(A->GetActorLocation(), Player->GetActorLocation());
		float DistB = FVector::DistSquared(B->GetActorLocation(), Player->GetActorLocation());
		return DistA < DistB;
	});
	
	if (InSpawnPoints.Num() < BestSpawnPointsAmount)
	{
		BestSpawnPointsAmount = InSpawnPoints.Num();
	}
	//배열에서 필요 없는 걸 제거 -> 요소 이동 발생
	//InSpawnPoints에서  PerSpawnPoints만큼 복사해서 사용하기 vs 원본에서 필요 없는거 삭제하기(InSpawnPoints.RemoveAt(PerSpawnPoints-1,PerSpawnPoints, false);)
	//PerSpawnPoints가 모든 포인트들의 과반수는 넘지 않을거 같고, 삭제가 많으면 비효율 적이라고 판단-> 필요한만큼 복사해서 쓰기로 함

	TArray<TSoftObjectPtr<AAnimalSpawnPoint>> OutSpawnPoints;
	for (int32 i = 0; i < BestSpawnPointsAmount; i++)
	{
		OutSpawnPoints.Add(InSpawnPoints[i]);
	}
	return OutSpawnPoints;
}



//분할생성 : 거리체크 -> 생성 필요 -> TryCreateQueue -> AddSpawnQueue -> Tick에서 TickSpawnQueue ->GetRandomLocationInSpawnVolume
void AAnimalSpawner::TryCreateQueue(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints)
{
	// 로드 중일 때는 새로 생성하지 않음
	if (bIsLoading)
	{
		return;
	}
	
	for (TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint : InSpawnPoints)
	{
		if (!SpawnPoint->GetIsCreated())  //포인트 영역에 생성한적이 없다면 생성
		{
			for (FAnimalSpawnInfo& Info : AnimalsInfo)
			{
				//if (SpawnPoint->태그컨테이너에 Info의 동물태그가 일치하면 생성 가능 ) -> 특정 포인트에서 특정 동물만 생성할 수 있도록
				CreateAnimalsQueue(Info,SpawnPoint);
			}
			SpawnPoint->SetIsCreated(true);
		}
		else
		{
			//스포너에서 생성된 총 동물 수 중 죽어서 리스폰대기열에 PermittedToSpawnLimit 이상 쌓이면 리스폰
			TrySpawnEntire();
				
			
		}
	}
}

void AAnimalSpawner::CreateAnimalsQueue(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint)
{
	AddCreateQueue(Info, InSpawnPoint, Info.LeaderCount, "Animal.Role.Leader");
	AddCreateQueue(Info, InSpawnPoint, Info.FollowCount, "Animal.Role.Follower");
	AddCreateQueue(Info, InSpawnPoint, Info.AloneCount , "Animal.Role.Alone");
}

void AAnimalSpawner::AddCreateQueue(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint, int32 Count, FName RoleTag)
{
	//해당하는 수 만큼 큐에 넣기
	for (int32 i=0; i < Count; i++)
	{
		FAnimalInitInfo InitInfo = GetRandomLocationInSpawnVolume(InSpawnPoint);
		FAnimalQueueInfo PerAnimalQueueInfo;
		PerAnimalQueueInfo.AnimalClass = Info.AnimalClass;
		PerAnimalQueueInfo.InitInfo = InitInfo;
		PerAnimalQueueInfo.RoleTag = RoleTag;
		PerAnimalQueueInfo.SpawnInfoIndex = AnimalsInfo.IndexOfByPredicate([&Info](const FAnimalSpawnInfo& PerSpawnInfo) // PerSpawnInfo: 배열에서 하나씩 순회하면서 넘겨받는 요소
		{
			return &PerSpawnInfo == &Info; //주의 : Info는 항상 참조로 받아와야함  
		});

		if (PerAnimalQueueInfo.SpawnInfoIndex == -1)
		{
			continue;
		}

		CreateInfoQueue.Enqueue(PerAnimalQueueInfo);
	}
}

void AAnimalSpawner::TickCreateQueue(TQueue<FAnimalQueueInfo>& InQueue, bool& InIsLoading)
{
	int32 SpawnedThisFrame = 0;
	while (!InQueue.IsEmpty() && SpawnedThisFrame < MaxSpawnPerTick)
	{
		FAnimalQueueInfo PerAnimal;
		InQueue.Dequeue(PerAnimal); //큐에서 맨앞 꺼내고 삭제

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABaseAIAnimal* Spawned = GetWorld()->SpawnActor<ABaseAIAnimal>(PerAnimal.AnimalClass,
													PerAnimal.InitInfo.Location, PerAnimal.InitInfo.Rotation, Params);
		if (!IsValid(Spawned))
		{
			continue;
		}
		
		MakeRandomActiveAtNight();
		Spawned->SetRoleTag(PerAnimal.RoleTag);
		Spawned->SetState(bIsShouldSleep);
		AnimalsInfo[PerAnimal.SpawnInfoIndex].SpawnAnimals.Emplace(Spawned);
		++SpawnedThisFrame;
	}
	if (InIsLoading && LoadInfoQueue.IsEmpty())
	{
		InIsLoading = false; // 로드 완료 후 false
	}
}

//디스폰 : SortFarthestAnimal(정렬 + 큐에 넣기) -> Tick에서 TickDespawnQueue ->ProcessDespawnQueue(해당하는 구조체 찾고, 숨김 TSet으로 이동, 애니멀 숨김처리)
void AAnimalSpawner::SortFarthestAnimal()
{
	TArray<TSoftObjectPtr<ABaseAIAnimal>> SortFarAnimals;
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	
	for (FAnimalSpawnInfo& Info : AnimalsInfo)
	{
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.SpawnAnimals)
		{
			if (Animal.IsValid())
			{
				SortFarAnimals.Add(Animal);
			}
		}
	}
	
	//먼순서대로 정렬
	SortFarAnimals.Sort([Player](const TSoftObjectPtr<ABaseAIAnimal>& A, const TSoftObjectPtr<ABaseAIAnimal>& B)
	{
		float DistA = FVector::DistSquared(A->GetActorLocation(), Player->GetActorLocation());
		float DistB = FVector::DistSquared(B->GetActorLocation(), Player->GetActorLocation());
		return DistA > DistB;
	});

	//디스폰 큐에 넣기
	for (TSoftObjectPtr<ABaseAIAnimal>& Animal : SortFarAnimals)
	{
		if (!Animal->IsHidden()) //살아있거나 파밍대기 상태인 애들 디스폰 처리
		{
			DespawnQueue.Enqueue(Animal);
		}
	}
}

void AAnimalSpawner::TickDespawnQueue()
{
	int32 DespawnedThisFrame = 0;
	while (!DespawnQueue.IsEmpty() && DespawnedThisFrame < MaxDespawnPerTick)
	{
		TSoftObjectPtr<ABaseAIAnimal> PerAnimal;
		DespawnQueue.Dequeue(PerAnimal); //큐에서 맨앞 꺼내고 삭제

		if (!PerAnimal.IsValid())
		{
			return;
		}
		ProcessDespawnQueue(PerAnimal);
		++DespawnedThisFrame;
	}
}

void AAnimalSpawner::ProcessDespawnQueue(TSoftObjectPtr<ABaseAIAnimal>& InAnimal)
{
	//살아있거나 파밍대기 상태인 애들
	if (InAnimal)
	{
		for (FAnimalSpawnInfo& Info : AnimalsInfo)
		{
			if (Info.SpawnAnimals.Contains(InAnimal.Get()))
			{
				InAnimal->SetHiddenInGame();
				break;
			}
		}
	}
}


//스폰 : 거리체크 -> 생성 필요없음 -> TrySpawnEntire -> TrySpawnAlive 또는 TrySpawnAlive + TrySpawnDead -> TickSpawnQueue
void AAnimalSpawner::TrySpawnEntire()
{
	// 디스폰 -> 스폰 전환시
	//살아있던 애들만 다시 스폰
	//살아있는 애들 + 죽은 애들까지 전체 스폰
	//게임 규칙에 따라 여기서 함수 추가
	
	TrySpawnAlive(AnimalsInfo);
	TrySpawnDead(AnimalsInfo);
}

void AAnimalSpawner::TrySpawnAlive(TArray<FAnimalSpawnInfo>& InfoArray)
{
	// 살아있었는데 숨겨졌던 애만 보이게 
	for (FAnimalSpawnInfo& Info : InfoArray)
	{
		TArray<TSoftObjectPtr<ABaseAIAnimal>> ToMove;
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.SpawnAnimals)
		{
			if (!Animal.IsValid())
			{
				continue;
			}
			
			if (Animal->IsHidden()) 
			{
				SpawnQueue.Enqueue(Animal);
			}
		}
	}
}

void AAnimalSpawner::TrySpawnDead(TArray<FAnimalSpawnInfo>& InfoArray)
{
	//죽었고 파밍시간도 끝나서 완전히 숨겨진 애들
	
	for (FAnimalSpawnInfo& Info : InfoArray)
	{
		if (Info.DeadAnimals.Num() < PermittedToSpawnLimit)
		{
			continue;
		}
		
		TArray<TSoftObjectPtr<ABaseAIAnimal>> ToMove;
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.DeadAnimals)
		{
			if (!Animal.IsValid())
			{
				continue;
			}
			
			SpawnQueue.Enqueue(Animal);
			ToMove.Emplace(Animal);
		}
		//대기열 이동
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : ToMove)
		{
			Info.DeadAnimals.Remove(Animal);
			Info.SpawnAnimals.Add(Animal);
		}
	}
}

void AAnimalSpawner::TickSpawnQueue()
{
	//다시 보이개만 처리
	
	int32 SpawnedThisFrame = 0;
	while (!SpawnQueue.IsEmpty() && SpawnedThisFrame < MaxSpawnPerTick)
	{
		TSoftObjectPtr<ABaseAIAnimal> PerAnimal;
		SpawnQueue.Dequeue(PerAnimal); //큐에서 맨앞 꺼내고 삭제
		if (!PerAnimal.IsValid())
		{
			continue;
		}
		MakeRandomActiveAtNight();
		PerAnimal->SetState(bIsShouldSleep);
		PerAnimal->SetVisibleInGame();
		
		++SpawnedThisFrame;
	}
}

void AAnimalSpawner::TryReleaseEntire()
{
	//타이머 일시정지-> 남은 시간부터 재시작 -> 스포너 매니저 가능해지면 킬 것
	// if (GetWorld()->GetTimerManager().IsTimerActive(DistanceTimerHandle))
	// {
	// 	GetWorld()->GetTimerManager().PauseTimer(DistanceTimerHandle);
	// }
	

	
	//큐 정리
	LoadInfoQueue.Empty();
	CreateInfoQueue.Empty();
	SpawnQueue.Empty();
	DespawnQueue.Empty();

	//인포 돌면서 생성한 동물들 해제
	for (int32 InfoIndex = 0; InfoIndex < AnimalsInfo.Num(); ++InfoIndex)
	{
		FAnimalSpawnInfo& Info = AnimalsInfo[InfoIndex];
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.SpawnAnimals)
		{
			if (!Animal.IsValid())
			{
				continue;
			}
			
			Animal->Destroy();
			Animal = nullptr;
		}
		Info.SpawnAnimals.Empty();
		
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.DeadAnimals)
		{
			if (!Animal.IsValid())
			{
				continue;
			}
			Animal->Destroy();
			Animal = nullptr;
		}
		Info.DeadAnimals.Empty();
	}

	for (TSoftObjectPtr<AAnimalSpawnPoint>& Point : SpawnPoints)
	{
		if (!Point.IsValid())
		{
			continue;
		}
		Point->SetIsCreated(false);
	}
}

//일괄생성 : 거리체크 -> 생성 필요 -> TryCreateEntire -> CreateAnimals -> SpawnAnimalWithTag ->GetRandomLocationInSpawnVolume
void AAnimalSpawner::TryCreateEntire(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints )
{
	for (TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint : InSpawnPoints)
	{
		//해당 스포너 구역에서 나타날 수 있는 동물 정보 담고 있는 구조체 순회하면서 생성 -> Init
		for (FAnimalSpawnInfo& Info : AnimalsInfo)
		{
			CreateAnimals(Info,SpawnPoint);
		}
	}
}

void AAnimalSpawner::CreateAnimals(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint)
{
	SpawnAnimalWithTag(Info, InSpawnPoint, "Animal.Role.Leader", Info.LeaderCount);
	SpawnAnimalWithTag(Info, InSpawnPoint, "Animal.Role.Follower", Info.FollowCount);
	SpawnAnimalWithTag(Info, InSpawnPoint, "Animal.Role.Alone", Info.AloneCount);
}

void AAnimalSpawner::SpawnAnimalWithTag(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint, FName RoleTag, int32 Count)
{
	for (int i = 0; i < Count; ++i)
	{
		FAnimalInitInfo InitInfo = GetRandomLocationInSpawnVolume(SpawnPoint);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABaseAIAnimal* SpawnedAnimal = GetWorld()->SpawnActor<ABaseAIAnimal>(Info.AnimalClass, InitInfo.Location, InitInfo.Rotation, SpawnParams);
		if (!IsValid(SpawnedAnimal))
		{
			continue;
		}

		SpawnedAnimal->SetRoleTag(RoleTag);
		Info.SpawnAnimals.Emplace(SpawnedAnimal); 
	}
}

FAnimalInitInfo AAnimalSpawner::GetRandomLocationInSpawnVolume(TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint)
{
	FVector SpawnLocation = SpawnPoint->GetActorLocation();
	UBoxComponent* BoxComp = SpawnPoint->FindComponentByClass<UBoxComponent>();
	FVector BoxExtent = BoxComp->GetScaledBoxExtent();
	int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomX = RandomSign * FMath::RandRange(-BoxExtent.X, BoxExtent.X);
	RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomY = RandomSign * FMath::RandRange(-BoxExtent.Y, BoxExtent.Y);
	SpawnLocation += FVector(RandomX, RandomY, 0.f);

	FRotator SpawnRotation = FRotator::ZeroRotator;
	SpawnRotation.Yaw = FMath::RandRange(-150.f, 150.f);

	FAnimalInitInfo InitInfo;
	InitInfo.Location = SpawnLocation;
	InitInfo.Rotation = SpawnRotation;
	
	return InitInfo;
}

void AAnimalSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UGameplayEventSubsystem* EventSystem = UGameplayEventSubsystem::GetGameplayEvent(GetWorld()))
	{
		EventSystem->OnGameEvent.RemoveDynamic(this, &AAnimalSpawner::OnGameTimeChanged);
	}
	
	GetWorldTimerManager().ClearTimer(DistanceTimerHandle);
	UMessageBus::GetInstance()->Unsubscribe(TEXT("HideAnimal"), MessageDelegateHandle);

	Super::EndPlay(EndPlayReason);
}


void AAnimalSpawner::OnTokenRaidEvent(int32 InGroupsPerWave, int32 inUnitsPerGroup, TSubclassOf<ABaseAIAnimal> InClass)
{
	for (int i = 0; i < InGroupsPerWave; i++)
	{
		FAnimalSpawnInfo Info;
		Info.AnimalClass = InClass;
		Info.TotalCount = inUnitsPerGroup;
		Info.LeaderCount = 1;
		Info.FollowCount = inUnitsPerGroup-1;
	
		AnimalsInfoByToken.Add(Info); //생성되는 객체들 정보를 담음, 인덱스 각 하나는 웨이브에서 한 그룹에 해당하는 정보
	}

	AddCreateQueueByLocation(AnimalsInfoByToken);
}

void AAnimalSpawner::AddCreateQueueByLocation(TArray<FAnimalSpawnInfo>& InfoByTokenArray)
{
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	//한 웨이브에 생성되는 그룹 수만큼 돌면서
	for (int32 i=0; i <InfoByTokenArray.Num() ; i++)
	{
		for (int32 j =0; j<InfoByTokenArray[i].TotalCount; j++)
		{
			FAnimalInitInfo InitInfo = GetRandomLocationByToken(Player->GetActorLocation());
			FAnimalQueueInfo PerAnimalQueueInfo;
			PerAnimalQueueInfo.AnimalClass = InfoByTokenArray[i].AnimalClass;
			PerAnimalQueueInfo.InitInfo = InitInfo;
			FName RoleTag = "Animal.Role.Follower";
			if (j==0)
			{
				RoleTag = "Animal.Role.Leader"; // 그룹에서 첫번째가 무조건 리더
			}
			PerAnimalQueueInfo.RoleTag = RoleTag;
			PerAnimalQueueInfo.SpawnInfoIndex = i;
			CreateInfoQueueByToken.Enqueue(PerAnimalQueueInfo); //동물들 행동을 위한 정보 담음
		}
	}
}
void AAnimalSpawner::TickCreateQueueByToken(TQueue<FAnimalQueueInfo>& InQueue)
{
	int32 SpawnedThisFrame = 0;
	while (!InQueue.IsEmpty() && SpawnedThisFrame < MaxSpawnPerTick)
	{
		FAnimalQueueInfo PerAnimal;
		InQueue.Dequeue(PerAnimal); //큐에서 맨앞 꺼내고 삭제

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABaseAIAnimal* Spawned = GetWorld()->SpawnActor<ABaseAIAnimal>(PerAnimal.AnimalClass,
													PerAnimal.InitInfo.Location, PerAnimal.InitInfo.Rotation, Params);
		if (!IsValid(Spawned))
		{
			continue;
		}
		
		Spawned->SetRoleTag(PerAnimal.RoleTag);
		Spawned->SetState(false);
		Spawned->SwitchBehaviorTree();
		Spawned->TriggerSpeedUp();
		AnimalsInfoByToken[PerAnimal.SpawnInfoIndex].SpawnAnimals.Emplace(Spawned); // 생성된 객체를 담음
		++SpawnedThisFrame;
	}

	//큐가 비었으면 AnimalsInfoByToken 쨰로 토큰 서브시스템한테 넘기기
	if (UTokenRaidSubsystem* TokenRaidSubsystem = GetGameInstance()->GetSubsystem<UTokenRaidSubsystem>())
	{
		TokenRaidSubsystem->RegisterWaitingArray(AnimalsInfoByToken); // 1웨이브에 쓰일 모든 객체들
	}
}

FAnimalInitInfo AAnimalSpawner::GetRandomLocationByToken(FVector PlayerLocation)
{
	FVector SpawnLocation = PlayerLocation;
	FVector Extent = FVector(1200.f,1200.f,0.f);
	int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomX = RandomSign * FMath::RandRange(-Extent.X, Extent.X);
	RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	const float RandomY = RandomSign * FMath::RandRange(-Extent.Y, Extent.Y);
	SpawnLocation += FVector(RandomX, RandomY, 0.f);

	FRotator SpawnRotation = FRotator::ZeroRotator;
	SpawnRotation.Yaw = FMath::RandRange(-150.f, 150.f);

	FAnimalInitInfo InitInfo;
	InitInfo.Location = SpawnLocation;
	InitInfo.Rotation = SpawnRotation;
	
	return InitInfo;
}


FGameplayTag AAnimalSpawner::GetIdentityTag() const
{
	return IdentityTag;
}