

#include "AnimalSpawner.h"

#include "AIController.h"
#include "AnimalSpawnPoint.h"
#include "BaseAIAnimal.h"
#include "GameplayTagAssetInterface.h"
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
	// 함수 바인딩
	MessageDelegateHandle = FMessageDelegate::CreateUObject(this, &ThisClass::ReceiveMessage);

	// FName으로 키값(메세지) 지정하고 델리게이트 전달
	UMessageBus::GetInstance()->Subscribe(TEXT("HideAnimal"), MessageDelegateHandle);
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
				Info.SpawnAnimals.Remove(Animal);
				Info.HiddenAnimals.Add(Animal);
				break;
			}
		}
	}
}

void AAnimalSpawner::MoveToHiddenFromSpawned()
{
	TSet<TSoftObjectPtr<ABaseAIAnimal>> ToMove;

	for (FAnimalSpawnInfo& Info : AnimalsInfo)
	{
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.SpawnAnimals)
		{
			if (Animal.IsValid() && Animal->IsHidden())
			{
				ToMove.Add(Animal);
				//여기서 바로 넣거나 삭제하면 Info.SpawnAnimals 또는 Info.HiddenAnimals 순회중 변경되는 불상사가 발생
			}
		}
		
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : ToMove)
		{
			Info.HiddenAnimals.Add(Animal);
			Info.SpawnAnimals.Remove(Animal);
		}
	}
}

void AAnimalSpawner::MoveToSpawnedFromHidden()
{
	TSet<TSoftObjectPtr<ABaseAIAnimal>> ToMove;

	for (FAnimalSpawnInfo& Info : AnimalsInfo)
	{
		for (TSoftObjectPtr<ABaseAIAnimal>& Animal : Info.SpawnAnimals)
		{
			if (Animal.IsValid() && Animal->IsHidden())
			{
				ToMove.Add(Animal);
			}
		}

		for (auto& Animal : ToMove)
		{
			Info.HiddenAnimals.Remove(Animal);
			Info.SpawnAnimals.Add(Animal);
		}
	}
}

void AAnimalSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickSpawnQueue();
	TickDespawnQueue();
}

void AAnimalSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearTimer(DistanceTimerHandle);
}

void AAnimalSpawner::DistanceCheck()
{
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player)
	{
		return;
	}
	float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
	if (SpawnDistance >= Distance)
	{ 
		CurDistResult = EAnimalSpawnerType::Spwan;
		
		TArray<TSoftObjectPtr<AAnimalSpawnPoint>> OutSpawnPoints = DiscardNearestPoint();
		TArray<TSoftObjectPtr<AAnimalSpawnPoint>> BestSpawnPoints = SelectNearPoints(OutSpawnPoints);
		TryCreateQueue(BestSpawnPoints);
	}
	else if (DespawnDistance <= Distance)
	{
		CurDistResult = EAnimalSpawnerType::Despawn;
		SortFarthestAnimal();
	}
	else if (ReleaseDistance <= Distance)
	{
		CurDistResult = EAnimalSpawnerType::Release;
	}
	else // 스폰과 디스폰 사이
	{
		CurDistResult = EAnimalSpawnerType::None;
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
	float MaxDistance = -1.f;
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
			DespawnQueue.Enqueue(Animal);
		}
	}
}

void AAnimalSpawner::TickDespawnQueue()
{
	int32 DespawnedThisFrame = 0;
	bool bIsEmpty = DespawnQueue.IsEmpty();
	while (bIsEmpty == false && DespawnedThisFrame < MaxDespawnPerTick)
	{
		TSoftObjectPtr<ABaseAIAnimal> PerAnimal;
		DespawnQueue.Dequeue(PerAnimal); //큐에서 맨앞 꺼내고 삭제
		ProcessDespawnQueue(PerAnimal);
		++DespawnedThisFrame;
	}
}

void AAnimalSpawner::ProcessDespawnQueue(TSoftObjectPtr<ABaseAIAnimal>& InAnimal)
{
	if (InAnimal)
	{
		for (FAnimalSpawnInfo& Info : AnimalsInfo)
		{
			if (Info.SpawnAnimals.Contains(InAnimal))
			{
				Info.SpawnAnimals.Remove(InAnimal);
				Info.HiddenAnimals.Add(InAnimal);
				InAnimal->SetHiddenInGame();
				break;
			}
		}
	}
}

//분할생성 : 거리체크 -> 생성 필요 -> TryCreateQueue -> AddSpawnQueue -> Tick에서 TickSpawnQueue ->GetRandomLocationInSpawnVolume
void AAnimalSpawner::TryCreateQueue(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints)
{
	for (TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint : InSpawnPoints)
	{
		if (SpawnPoint->GetAliveAnimalsInBox() <= PermittedToSpawnLimit) //포인트 영역에 PermittedToSpawnLimit 이하로 나와있다면 스폰
		{
			for (FAnimalSpawnInfo& Info : AnimalsInfo)
			{
				CreateQueueAnimals(Info,SpawnPoint);
			}
		}
	}
	//다른 지역 -> 스폰 지역 진입, 스폰이 일어난 경우
	PreDistResult = CurDistResult;
}

void AAnimalSpawner::CreateQueueAnimals(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint)
{
	AddSpawnQueue(Info, InSpawnPoint, "Animal.Group.Leader", Info.LeaderCount);
	AddSpawnQueue(Info, InSpawnPoint, "Animal.Group.Patrol", Info.PatrolCount);
	AddSpawnQueue(Info, InSpawnPoint, "Animal.Group.Follower", Info.FollowCount);
	AddSpawnQueue(Info, InSpawnPoint, "Animal.Group.Alone", Info.AloneCount);
}

void AAnimalSpawner::AddSpawnQueue(FAnimalSpawnInfo& Info, TSoftObjectPtr<AAnimalSpawnPoint>& InSpawnPoint, FName RoleTag, int32 Count)
{
	//해당하는 수 만큼 큐에 넣기
	for (int32 i=0; i < Count; i++)
	{
		FAnimalInitInfo InitInfo = GetRandomLocationInSpawnVolume(InSpawnPoint);
		
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

		AnimalInfoQueue.Enqueue(PerAnimalQueueInfo);
	}
}


void AAnimalSpawner::TickSpawnQueue()
{
	int32 SpawnedThisFrame = 0;
	bool bIsEmpty = AnimalInfoQueue.IsEmpty();
	while (bIsEmpty == false && SpawnedThisFrame < MaxSpawnPerTick)
	{
		FAnimalQueueInfo PerAnimal;
		AnimalInfoQueue.Dequeue(PerAnimal); //큐에서 맨앞 꺼내고 삭제

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABaseAIAnimal* Spawned = GetWorld()->SpawnActor<ABaseAIAnimal>(PerAnimal.AnimalClass,
													PerAnimal.InitInfo.Location, PerAnimal.InitInfo.Rotation, Params);
		if (!IsValid(Spawned))
		{
			continue;
		}

		Spawned->GetGameplayTagContainer().AddTag(FGameplayTag::RequestGameplayTag(PerAnimal.RoleTag));

		if (AAIController* AIController = Cast<AAIController>(Spawned->GetController()))
		{
			AIController->GetBlackboardComponent()->SetValueAsName("NGroupTag", PerAnimal.RoleTag);
		}

		AnimalsInfo[PerAnimal.SpawnInfoIndex].SpawnAnimals.Emplace(Spawned);
		++SpawnedThisFrame;
	}
}


//일괄생성 : 거리체크 -> 생성 필요 -> TryCreateEntire -> CreateAnimals -> SpawnAnimalWithTag ->GetRandomLocationInSpawnVolume
void AAnimalSpawner::TryCreateEntire(TArray<TSoftObjectPtr<AAnimalSpawnPoint>>& InSpawnPoints )
{
	//플레이어가 계속 스폰 지역에 있는 경우
	if ( PreDistResult == EAnimalSpawnerType::Spwan && CurDistResult == PreDistResult)
	{
		return;
	}

	for (TSoftObjectPtr<AAnimalSpawnPoint>& SpawnPoint : InSpawnPoints)
	{
		//해당 스포너 구역에서 나타날 수 있는 동물 정보 담고 있는 구조체 순회하면서 생성 -> Init
		for (FAnimalSpawnInfo& Info : AnimalsInfo)
		{
			CreateAnimals(Info,SpawnPoint);
		}
	}
	//다른 지역 -> 스폰 지역 진입, 스폰이 일어난 경우
	PreDistResult = CurDistResult;
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
	
		SpawnedAnimal->GetGameplayTagContainer().AddTag(FGameplayTag::RequestGameplayTag(RoleTag));

		if (AAIController* AIController = Cast<AAIController>(SpawnedAnimal->GetController()))
		{
			AIController->GetBlackboardComponent()->SetValueAsName("NGroupTag", RoleTag);
		}
		
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

void AAnimalSpawner::TrySpawnEntire()
{
	//전체 다시 보이게

	
	{
		//
		// int32 AnimalsInfoIndex = 0;
		// int32 SpawnAnimalsMinimum = 100;
		//
		// //가장 적게 필드에 나와 있는 동물이 스폰된다
		// for (int i=0; i < AnimalsInfo.Num(); i++)
		// {
		// 	if (AnimalsInfo[i].SpawnAnimals.Num() < SpawnAnimalsMinimum)
		// 	{
		// 		SpawnAnimalsMinimum = AnimalsInfo[i].SpawnAnimals.Num();
		// 		AnimalsInfoIndex = i;
		// 	}
		// }
	}

	// for (int i=0; i<AnimalsInfo.Num(); i++)
	// {
	// 	//숨겨진 수 만큼만 다시 스폰
	// 	for (const TSoftObjectPtr<ABaseAIAnimal>& HiddenAnimal : AnimalsInfo[i].HiddenAnimals)
	// 	{
	// 		FVector SpawnLocation = SpawnPoints[SpawnPointIndex]->GetActorLocation();
	// 		UBoxComponent* BoxComp = SpawnPoints[SpawnPointIndex]->FindComponentByClass<UBoxComponent>();
	// 		FVector BoxExtent = BoxComp->GetScaledBoxExtent();
	// 	
	// 		int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	// 		const float RandomX = RandomSign * FMath::RandRange(-BoxExtent.X, BoxExtent.X);
	// 		RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
	// 		const float RandomY = RandomSign * FMath::RandRange(-BoxExtent.Y, BoxExtent.Y);
	// 		SpawnLocation += FVector(RandomX, RandomY, 0.f);
	//
	// 	
	// 		HiddenAnimal->SetActorLocation(SpawnLocation);
	// 	
	// 		HiddenAnimal->SetVisibleInGame();
	// 	}
	// }

}

void AAnimalSpawner::TrySpawnPart()
{
	//일부만 다시 보이게
}


void AAnimalSpawner::TryReleaseEntire()
{
	//전체 메모리 해제
}
