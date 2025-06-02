

#include "AnimalSpawner.h"

#include "AIController.h"
#include "AnimalSpawnPoint.h"
#include "BaseAIAnimal.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"


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
}

void AAnimalSpawner::ReceiveMessage(const FName MessageType, UObject* Payload)
{
	if (TEXT("HideAnimal") == MessageType)
	{
		//데스 어빌리티에서 몽타주 재생 끝났을 때 메세지 버스로 신호를 받고 애니멀쪾에서는 숨김, tick, collision, PC,BB 등등 정지
		//스포너에서는 단순 스폰드 -> 히든으로 저장 위치 이동만, 나중에 세이브할 때는 저장 전처리 때 필터 함수 돌려서 최종확인하기
		
		MessageMoveToHidden(Payload);
	}
}

void AAnimalSpawner::MessageMoveToHidden(UObject* Payload)
{
	if (ABaseAIAnimal* Animal = Cast<ABaseAIAnimal>(Payload))
	{
		for (FAnimalSpawnInfo& Info : AnimalsInfo)
		{
			if (Info.SpawnAnimals.Contains(Animal))
			{
				UE_LOG(LogTemp, Warning, TEXT("[Hidden] Animal %s (%p)"), *Animal->GetName(), Animal);
				
				Info.SpawnAnimals.Remove(Animal);
				Info.HiddenAnimals.Add(Animal);
				return;
			}
		}
	}
}

void AAnimalSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!LoadInfoQueue.IsEmpty())
	{
		TickCreateQueue(LoadInfoQueue, bIsLoading);
	}
	else
	{
		TickCreateQueue(CreateInfoQueue, bIsLoading);
	}
	TickSpawnQueue();
	TickDespawnQueue();
}

void AAnimalSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(DistanceTimerHandle);
	UMessageBus::GetInstance()->Unsubscribe(TEXT("HideAnimal"), MessageDelegateHandle);
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
			if (Animal.IsValid() && !Animal->IsHidden())
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
	

	//포인트들 순회 -> 가장 가까우 포인트만 스폰 지역에서 제외
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
		if (!Animal->IsHidden()) //살아있는데 거리때문에 디스폰된 애들이 아니라면 디스폰 처리
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
		DespawnQueue.Dequeue(PerAnimal); //큐에서 맨앞 꺼내고 삭제, 객체 생성이 아예 진행되지 않은 상태에서 처음 생성할 때

		if (!PerAnimal.IsValid())
		{
			return;
		}
		ProcessDespawnQueue(PerAnimal); // 거리 멀어지면 숨김처리할때, 죽은 애들은 이미 메세지버스로 숨김처리, 살아있는 애들 디스폰 구분이 필요해짐-> 숨기지만 대기열 이동은 하지않음
		++DespawnedThisFrame;
	}
}

void AAnimalSpawner::ProcessDespawnQueue(TSoftObjectPtr<ABaseAIAnimal>& InAnimal)
{
	// 거리 멀어지면 숨김처리할때, 죽은 애들은 이미 메세지버스로 숨김처리, 살아있는 애들 디스폰 구분이 필요해짐-> 숨기지만 대기열 이동은 하지않음
	if (InAnimal)
	{
		for (FAnimalSpawnInfo& Info : AnimalsInfo)
		{
			if (Info.SpawnAnimals.Contains(InAnimal.Get()))
			{
				//Info.SpawnAnimals.Remove(InAnimal); //여기 수정함
				//Info.HiddenAnimals.Add(InAnimal);   //여기 수정함
				InAnimal->SetHiddenInGame();
				
				UE_LOG(LogTemp, Log, TEXT("Despawn Animal pointer: %p"), InAnimal.Get());
				break;
			}
		}
	}
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
		else//이미 create됐던 지역이라면 숨겨진 애들만 다시 보이게 처리로 바뀌어야함
		{
			if (SpawnPoint->GetAliveAnimalsInBox() < PermittedToSpawnLimit) //애들이 다 튀어나가서 포인트 영역안에 사냥하고 잇는 애만 남으면 -> 그녀석 죽이면 바로 재스폰 되버림  -> 영역 제한하거나 포인트가 그 포인트에서 생성된 애들 수나 포인터 들고 있거나
			{
				UE_LOG(LogTemp, Warning, TEXT("SpawnPoint [%s]: Overlapping alive animals: %d"), *SpawnPoint->GetName(), SpawnPoint->GetAliveAnimalsInBox());
				//타이머 기반 스폰 기능 넣으면 타이머에 TrySpawnDead(AnimalsInfo) 만 바인딩 해서 죽은 애들만 다시 스폰 가능 
				TrySpawnEntire();
			}
		}
	}
}

void AAnimalSpawner::CreateAnimalsQueue(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint)
{
	AddCreateQueue(Info, InSpawnPoint, Info.LeaderCount, "Animal.Group.Leader");
	AddCreateQueue(Info, InSpawnPoint, Info.PatrolCount, "Animal.Group.Patrol");
	AddCreateQueue(Info, InSpawnPoint, Info.FollowCount, "Animal.Group.Follower");
	AddCreateQueue(Info, InSpawnPoint, Info.AloneCount , "Animal.Group.Alone");
}

void AAnimalSpawner::AddCreateQueue(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint, int32 Count, FName RoleTag)
{
	//해당하는 수 만큼 큐에 넣기
	for (int32 i=0; i < Count; i++)
	{
		FAnimalInitInfo InitInfo = GetRandomLocationInSpawnVolume(InSpawnPoint);
		FAnimalQueueInfo PerAnimalQueueInfo; //여기수정함
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
		Spawned->SetRoleTag(PerAnimal.RoleTag);
		Spawned->SetIdleState();
		AnimalsInfo[PerAnimal.SpawnInfoIndex].SpawnAnimals.Emplace(Spawned);
		++SpawnedThisFrame;
	}
	if (InIsLoading && LoadInfoQueue.IsEmpty())
	{
		InIsLoading = false; // 로드 완료 후 false
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
	SpawnAnimalWithTag(Info, InSpawnPoint, "Animal.Group.Leader", Info.LeaderCount);
	SpawnAnimalWithTag(Info, InSpawnPoint, "Animal.Group.Patrol", Info.PatrolCount);
	SpawnAnimalWithTag(Info, InSpawnPoint, "Animal.Group.Follower", Info.FollowCount);
	SpawnAnimalWithTag(Info, InSpawnPoint, "Animal.Group.Alone", Info.AloneCount);
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
	// 거리 멀어지면 숨김처리할때, 죽은 애들은 이미 메세지버스로 숨김처리, 살아있는 애들 디스폰 구분이 필요해짐-> 숨기지만 대기열 이동은 하지않음
	//애니멀->SetvitableInGame (공통)
	//HiddenAnimals 면 대기열 이동
	
	for (FAnimalSpawnInfo& Info : InfoArray)
	{
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.SpawnAnimals)
		{
			if (!Animal.IsValid())
			{
				continue;
			}
			if (Animal->IsHidden())
			{
				//add 큐 -> 다시 보이개만 처리
				SpawnQueue.Enqueue(Animal);
			}
		}
	}
}

void AAnimalSpawner::TrySpawnDead(TArray<FAnimalSpawnInfo>& InfoArray)
{
	for (FAnimalSpawnInfo& Info : InfoArray)
	{
		TArray<TSoftObjectPtr<ABaseAIAnimal>> ToMove;
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.HiddenAnimals)
		{
			if (!Animal.IsValid())
			{
				continue;
			}
			//add 큐 -> 다시 보이개만 처리
			SpawnQueue.Enqueue(Animal);
			ToMove.Emplace(Animal);
		}
		//대기열 이동
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : ToMove)
		{
			Info.HiddenAnimals.Remove(Animal);
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
	// 	UE_LOG(LogTemp, Warning, TEXT("DistanceTimerHandle paused."));
	// }
	
	//전체 메모리 해제
	UE_LOG(LogTemp, Warning, TEXT("=== TryReleaseEntire: Start ==="));
	
	//큐 정리
	LoadInfoQueue.Empty();
	CreateInfoQueue.Empty();
	SpawnQueue.Empty();
	DespawnQueue.Empty();

	//인포 돌면서 생성한 동물들 해제
	for (int32 InfoIndex = 0; InfoIndex < AnimalsInfo.Num(); ++InfoIndex)
	{
		FAnimalSpawnInfo& Info = AnimalsInfo[InfoIndex];
        UE_LOG(LogTemp, Warning, TEXT("--- Info[%d] ---"), InfoIndex);
        UE_LOG(LogTemp, Warning, TEXT("SpawnAnimals Count: %d"), Info.SpawnAnimals.Num());
        		
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
		UE_LOG(LogTemp, Warning, TEXT("SpawnAnimals Count: %d"), Info.SpawnAnimals.Num());

		UE_LOG(LogTemp, Warning, TEXT("SpawnAnimals Count: %d"), Info.HiddenAnimals.Num());
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.HiddenAnimals)
		{
			if (!Animal.IsValid())
			{
				continue;
			}
			Animal->Destroy();
			Animal = nullptr;
		}
		Info.HiddenAnimals.Empty();
		UE_LOG(LogTemp, Warning, TEXT("SpawnAnimals Count: %d"), Info.HiddenAnimals.Num());
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
