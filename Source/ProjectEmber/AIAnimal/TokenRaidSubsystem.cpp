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

void UTokenRaidSubsystem::RegisterWaitingArray(TArray<FAnimalSpawnInfo>& InArray)
{
	WaitingArray = InArray;
	RunEQSByQuerier();
}

//리더만 eqs 실행 -> 베스트 위치 받아오고 -> 랜덤 포인트 생성된 애들이 담긴 WaitingArray 인덱스 안의 동물별로 위치 지정
void UTokenRaidSubsystem::RunEQSByQuerier()
{
	for (int32 i= 0; i<WaitingArray.Num(); i++)
	{
		for (auto& Animal : WaitingArray[i].SpawnAnimals)
		{
			if ("Animal.Role.Leader" == Animal->GetRoleTag())
			{
				FoundQueriers.Emplace(i,Animal.Get());
			}
		}
	}
	
	//리더 없을 때 예외 처리해야함 -> 리더 없으면 다음 아무나 BB 그룹 태그, 롤 태그 리더로 바꿔야함
	
	for (auto Querier : FoundQueriers)
	{
		APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
		Cast<AAIAnimalController>(Querier.Value->GetController())->GetBlackboardComponent()->SetValueAsObject("TargetActor", Player);
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
																	GetWorld(),
																	EQSQuery.Get(),
																	Cast<UObject>(Querier.Value),
																	EEnvQueryRunMode::AllMatching,
																	nullptr);
	
		if (QueryInstance)
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &UTokenRaidSubsystem::OnEQSComplete);
		}
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
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector PlayerLocation = Player->GetActorLocation();
	int32 index =0;
	for (TTuple<int32,ABaseAIAnimal*> Querier : FoundQueriers)
	{
		for (auto& Animal : WaitingArray[Querier.Key].SpawnAnimals)
		{
			Cast<AAIAnimalController>(Animal->GetController())->GetBlackboardComponent()->SetValueAsVector("SafeLocation", FoundLocations[index]);
			Cast<AAIAnimalController>(Animal->GetController())->GetBlackboardComponent()->SetValueAsName("NStateTag","Animal.State.Idle");
			OriLocation.Emplace(Animal.Get(), FoundLocations[index]);
			index++;
		}
	}
}
void UTokenRaidSubsystem::OnFirstMovementComplete(AActor* InUnit, bool InResult)
{
	if (!InResult)
	{
		return;
	}
	ABaseAIAnimal* Unit = Cast<ABaseAIAnimal>(InUnit);
	ReadyUnits.AddUnique(Unit);
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector PlayerLocation = Player->GetActorLocation();
	Cast<AAIAnimalController>(Unit->GetController())->GetBlackboardComponent()->SetValueAsName("NStateTag","Animal.State.Attack");
	Cast<AAIAnimalController>(Unit->GetController())->GetBlackboardComponent()->SetValueAsObject("TargetActor",Player);
	if (ReadyUnits.Num() == 3 && CanActiveTokens != 0)
	{
		GiveTokenToRandom();
	}
}

void UTokenRaidSubsystem::OnMovementComplete(AActor* InUnit, bool InResult)
{
	if (!InResult)
	{
		return;
	}
	ABaseAIAnimal* Unit = Cast<ABaseAIAnimal>(InUnit);
	ReadyUnits.AddUnique(Unit);
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector PlayerLocation = Player->GetActorLocation();
	Cast<AAIAnimalController>(Unit->GetController())->GetBlackboardComponent()->SetValueAsName("NStateTag","Animal.State.Attack");
}

void UTokenRaidSubsystem::GiveTokenToRandom()
{
	if (CanActiveTokens == 0) // 줄 수 있는 토큰이 없으면 리턴
	{
		UE_LOG(LogTemp, Warning, TEXT("No eligible units to give token."));
		return;
	}

	// 랜덤 인덱스 선택
	int32 RandomIndex = FMath::RandRange(0, ReadyUnits.Num() - 1);

	ABaseAIAnimal* SelectedUnit = ReadyUnits[RandomIndex];
	if (!IsValid(SelectedUnit))
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected unit is invalid."));
		return;
	}

	// 토큰 부여
	CanActiveTokens--;
	ReadyUnits.RemoveAt(RandomIndex);
	ActiveUnits.AddUnique(SelectedUnit);
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	FVector PlayerLocation = Player->GetActorLocation();
	Cast<AAIAnimalController>(SelectedUnit->GetController())->GetBlackboardComponent()->SetValueAsVector("TargetLocation", PlayerLocation);
	SelectedUnit->SetHasToken(true);
}

void UTokenRaidSubsystem::ReturnToken(ABaseAIAnimal* Unit)
{
	if (!Unit || !Unit->GetHasToken())
	{
		return;
	}
	CanActiveTokens++;
	Unit->SetHasToken(false);
	//여기서 레디에 추가하는 건 토큰이 유무에 따라 누락될 수 있어서 브로드캐스트(OnMovementComplete)로 처리됨
	ActiveUnits.Remove(Unit);

	
	if (CanActiveTokens != 0) 
	{
		GiveTokenToRandom(); // 다음 단계 진행
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