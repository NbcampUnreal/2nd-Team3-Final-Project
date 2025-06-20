// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/TokenRaidSubsystem.h"

#include "AnimalSpawner.h"
#include "BaseAIAnimal.h"
#include "TokenRaidInfo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Kismet/GameplayStatics.h"

void UTokenRaidSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EQSQuery = LoadObject<UEnvQuery>(nullptr, TEXT("/Game/_Blueprints/AI/Animal/EQS_BestPointQuery.EQS_BestPointQuery"));
	TokenRaidDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/_Blueprints/AI/Animal/DT_TokenRaidInfo.DT_TokenRaidInfo"));
	if (TokenRaidDataTable)
	{
		TArray<FTokenRaidInfo*> AllRows;
		TokenRaidDataTable->GetAllRows(TEXT("TokenRaidContext"), AllRows);

		for (FTokenRaidInfo* Row : AllRows)
		{
			if (Row)
			{
				CurrentRowArray.Add(*Row);
			}
		}
	}
}

FTokenRaidInfo UTokenRaidSubsystem::GetRaidInfoRow(FGameplayTag Region)
{
	for (auto Row : CurrentRowArray)
	{
		if (Row.Region == Region)
		{
			return Row;
		}
	}
	return FTokenRaidInfo();
}

void UTokenRaidSubsystem::RegisterRaidInfoArray(AAnimalSpawner* OwnerSpawner ,TArray<FAnimalSpawnInfo>& InArray)
{
	InfoOwnerSpawner = OwnerSpawner;
	RaidInfoArray = InArray;
	RunEQSByQuerier();
}

//리더만 eqs 실행 -> 베스트 위치 받아오고 -> 랜덤 포인트 생성된 애들이 담긴 RaidInfoArray 인덱스 안의 동물별로 위치 지정
void UTokenRaidSubsystem::RunEQSByQuerier()
{
	ABaseAIAnimal* Querier = nullptr;
	//스폰된 애들 통틀어서 대표로 EQS 돌릴 애 정함 -> [0]번째의  begin()
	for (auto& Info : RaidInfoArray)
	{
		if (Info.SpawnAnimals.Num() == 0) //한그룹이 전멸 당했으면 넘어감
		{
			continue;
		}
		if (IsValid(Info.SpawnAnimals.begin()->Get()))
		{
			Querier = Cast<ABaseAIAnimal>(Info.SpawnAnimals.begin()->Get());
		}
	}

	//다 돌았는데도 null이면 전멸 당한 것
	if (!Querier)
	{
		return; //레이드 종료 -> TryReleaseToken(); 불러서 정리 -> 웨이브처리어케함 누가 웨이브돌릴거야
	}
	//EQC에서 BB의 타겟엑터에서 플레이어를 가져다 쓰고 있으니까 EQS 돌리는 애는 타겟엑터 지정이 먼저 필요함
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	Cast<AAIAnimalController>(Querier->GetController())->GetBlackboardComponent()->SetValueAsObject("TargetActor", Player);

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
																GetWorld(),
																EQSQuery.Get(),
																Cast<UObject>(Querier),
																EEnvQueryRunMode::AllMatching,
																nullptr);

	if (QueryInstance)
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UTokenRaidSubsystem::OnEQSComplete);
	}
	
}

void UTokenRaidSubsystem::OnEQSComplete(UEnvQueryInstanceBlueprintWrapper* QueryInstance,EEnvQueryStatus::Type QueryStatus)
{
	if (EEnvQueryStatus::Success != QueryStatus)
	{
		return;
	}
	 //eqs 돌린 결과 모든 포인트들 담김
	if (QueryInstance)
	{
		FoundLocations.Empty(); // 받아오기 전에 함 비움
		QueryInstance->GetQueryResult()->GetAllAsLocations(FoundLocations);
		if (FoundLocations.Num() == 0)
		{
			return;
		}
		MovementStart(); // 베스트포인트 지정하면 bt에서 이동 노드 실행 -> 이동완료하면 델리게이트로 OnMovementComplete 호출됨
	}
}

void UTokenRaidSubsystem::MovementStart()
{
	int32 index =0;
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	
	for (auto& Info : RaidInfoArray)
	{
		for (auto& Animal : Info.SpawnAnimals)
		{
			Cast<AAIAnimalController>(Animal->GetController())->GetBlackboardComponent()->SetValueAsObject("TargetActor", Player);
			Cast<AAIAnimalController>(Animal->GetController())->GetBlackboardComponent()->SetValueAsVector("SafeLocation", FoundLocations[index]);
			//Cast<AAIAnimalController>(Animal->GetController())->GetBlackboardComponent()->SetValueAsName("NStateTag","Animal.State.Idle");
			OriLocation.Emplace(Animal.Get(), FoundLocations[index]);
			index++;
		}
	}
}

void UTokenRaidSubsystem::OnFirstMovementComplete(AActor* InUnit, bool InResult)
{
	//한번 실행될때마다 다 도착했는지 체크 -> 처음 대형 다 갖추고 시작해야함
	//토큰 발행
	if (!InResult)
	{
		return;
	}
	ABaseAIAnimal* Unit = Cast<ABaseAIAnimal>(InUnit);
	if (Unit->GetIsDead()) 
	{
		return;
	}
	ActiveUnits.AddUnique(Unit); // 도착했으면 대기열에 추가
	
	for (int32 i=0; i<ActiveUnits.Num(); i++)
	{
		if (ActiveUnits[i]->GetIsDead()) //도착하는 사이에 다른 하나가 죽어있으면, 처음 시작 타이밍이라 그럴일 없겠지만 방어코드 
		{
			ActiveUnits.RemoveAtSwap(i);
		}
	}
	
	int32 AliveCount = 0;
	for (auto& Info : RaidInfoArray)
	{
		for (auto& Animal : Info.SpawnAnimals)
		{
			if (!Animal->GetIsDead()) //준비되어야할 전체 수를 구함
			{
				AliveCount++;
			}
		}
	}
	
	if (ActiveUnits.Num() == AliveCount && CanActiveTokens > 0)
	{
		//복사본 안 만들면 for문 돌다가 원본 변경됨
		int32  CanActiveCount = CanActiveTokens;
		for (int32 i = 0; i < CanActiveCount; i++)
		{
			GiveTokenToRandom();
		}
	}
}

void UTokenRaidSubsystem::OnMovementComplete(AActor* InUnit, bool InResult)
{
	//개별적으로 도착하면 호출됨
	//다시 토큰 받을 수 있는 상태
	if (!InResult)
	{
		return;
	}
	
	ABaseAIAnimal* Animal = Cast<ABaseAIAnimal>(InUnit);
	if (Animal->GetIsDead()) 
	{
		return;
	}
	ActiveUnits.AddUnique(Animal); //파밍 대기 상태가 아니면 토큰 받을 수 있음
}

void UTokenRaidSubsystem::GiveTokenToRandom()
{
	if (CanActiveTokens == 0) // 줄 수 있는 토큰이 없으면 리턴
	{
		UE_LOG(LogTemp, Warning, TEXT("No eligible units to give token."));
		return;
	}

	
	// 랜덤 인덱스 선택
	int32 RandomIndex = FMath::RandRange(0, ActiveUnits.Num() - 1);
	if (!ActiveUnits.IsValidIndex(RandomIndex))
	{
		return;
	}
	
	TWeakObjectPtr<ABaseAIAnimal> SelectedUnit = ActiveUnits[RandomIndex];
	if (!SelectedUnit.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected unit is invalid."));
		return;
	}

	// 토큰 부여
	CanActiveTokens--;
	SelectedUnit->SetHasToken(true);
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector PlayerLocation = Player->GetActorLocation();
	Cast<AAIAnimalController>(SelectedUnit->GetController())->GetBlackboardComponent()->SetValueAsVector("TargetLocation", PlayerLocation);
	ActiveUnits.RemoveAtSwap(RandomIndex);
}

void UTokenRaidSubsystem::ReturnToken(ABaseAIAnimal* Unit)
{
	if (!Unit || !Unit->GetHasToken() || Unit->GetIsDead())
	{
		return;
	}
	
	CanActiveTokens++;
	Unit->SetHasToken(false);
	if (CanActiveTokens > 0) 
	{
		GiveTokenToRandom(); // 다음 단계 진행, 여기서 토큰대기열에 등록하면 안됨(돌아가는중임)
	}
}

FVector UTokenRaidSubsystem::GetBestLocation(ABaseAIAnimal& Animal)
{
	if (!OriLocation.Find(&Animal))
	{
		if (IsValid(&Animal))
		{
			return Animal.GetActorLocation();
		}
		return FVector::ZeroVector;
	}
	return *OriLocation.Find(&Animal);
}

void UTokenRaidSubsystem::TryReleaseToken()
{
	OriLocation.Empty();
	ActiveUnits.Empty();
	RaidInfoArray.Empty();
	InfoOwnerSpawner->TryReleaseToken();
}
