

#include "AnimalSpawner.h"

#include "AIController.h"
#include "AnimalSpawnPoint.h"
#include "BaseAIAnimal.h"
#include "GameplayTagAssetInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"


AAnimalSpawner::AAnimalSpawner()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
}

void AAnimalSpawner::BeginPlay()
{
	Super::BeginPlay();

	for (FAnimalSpawnInfo& Info : AnimalsInfo)
	{
		int32 Total = Info.TotalCount;
		int32 Leaders = Info.LeaderCount;
		int32 Patrols = Info.PatrolCount;
		int32 Follows = Info.FollowCount;
		int32 Alones = Info.AloneCount;
		
		for (int32 i = 0; i < Total; ++i)
		{
			FVector SpawnLocation = FVector::ZeroVector;
			FRotator SpawnRotation = FRotator::ZeroRotator;
			SpawnRotation.Yaw = FMath::RandRange(-150.f, 150.f);
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			ABaseAIAnimal* SpawnedAnimal = GetWorld()->SpawnActor<ABaseAIAnimal>(Info.AnimalClass, SpawnLocation, SpawnRotation, SpawnParams);
			Info.HiddenAnimals.Emplace(SpawnedAnimal); //TSet은 순서가 유지된다는 보장이 없다. 삽입삭제가 잦을수록 더
			
			if (IsValid(SpawnedAnimal))
			{
				if (Leaders > 0)
				{
					SpawnedAnimal->GetGameplayTagContainer().AddTag(FGameplayTag::RequestGameplayTag("Animal.Group.Leader"));
					Cast<AAIController>(SpawnedAnimal->GetController())->GetBlackboardComponent()->SetValueAsName("NGroupTag", "Animal.Group.Leader");
					Leaders--;
				}
				else if (Patrols > 0)
				{
					SpawnedAnimal->GetGameplayTagContainer().AddTag(FGameplayTag::RequestGameplayTag("Animal.Group.Patrol"));
					Cast<AAIController>(SpawnedAnimal->GetController())->GetBlackboardComponent()->SetValueAsName("NGroupTag", "Animal.Group.Patrol");
					Patrols--;
				}
				else if (Follows > 0)
				{
					SpawnedAnimal->GetGameplayTagContainer().AddTag(FGameplayTag::RequestGameplayTag("Animal.Group.Follower"));
					Cast<AAIController>(SpawnedAnimal->GetController())->GetBlackboardComponent()->SetValueAsName("NGroupTag", "Animal.Group.Follower");
					Follows--;
				}
				else
				{
					SpawnedAnimal->GetGameplayTagContainer().AddTag(FGameplayTag::RequestGameplayTag("Animal.Group.Alone"));
					Cast<AAIController>(SpawnedAnimal->GetController())->GetBlackboardComponent()->SetValueAsName("NGroupTag", "Animal.Group.Alone");
					Alones--;
					
				}
			}
		}
	}

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AAnimalSpawner::OnBeginOverlap);
}

void AAnimalSpawner::OnBeginOverlap(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	TrySpawnAnimals();
}

void AAnimalSpawner::TrySpawnAnimals()
{
	int32 SpawnPointIndex = 0;
	int32 SpawnPointMinimum = 100;
	
	//포인트들 순회하며 스폰 조건에 맞는 포인트에 생성
	// 구역 안에 플레이어가 없고, 가장 적은 동물이 남아있으면 스폰
	for (int i=0; i < SpawnPoints.Num(); i++)
	{
		if (SpawnPoints[i].IsValid())
		{
			if (SpawnPoints[i]->GetIsInHere())
			{
				continue;
			}
			if (const int32 AliveAnimals =  SpawnPoints[i]->GetAliveAnimalsInBox() < SpawnPointMinimum)
			{
				SpawnPointMinimum = AliveAnimals;
				SpawnPointIndex = i;
			}
		}
	}

	int32 AnimalsInfoIndex = 0;
	int32 SpawnAnimalsMinimum = 100;
	
	//가장 적게 필드에 나와 있는 동물이 스폰된다
	for (int i=0; i < AnimalsInfo.Num(); i++)
	{
		if (AnimalsInfo[i].SpawnAnimals.Num() < SpawnAnimalsMinimum)
		{
			SpawnAnimalsMinimum = AnimalsInfo[i].SpawnAnimals.Num();
			AnimalsInfoIndex = i;
		}
	}

	//숨겨진 수 만큼만 다시 스폰
	for (const TSoftObjectPtr<ABaseAIAnimal>& HiddenAnimal : AnimalsInfo[AnimalsInfoIndex].HiddenAnimals)
	{
		FVector SpawnLocation = SpawnPoints[SpawnPointIndex]->GetActorLocation();
		UBoxComponent* BoxComp = SpawnPoints[SpawnPointIndex]->FindComponentByClass<UBoxComponent>();
		FVector BoxExtent = BoxComp->GetScaledBoxExtent();
		
		int RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
		const float RandomX = RandomSign * FMath::RandRange(-BoxExtent.X, BoxExtent.X);
		RandomSign = FMath::RandRange(0, 1) == 0 ? -1 : 1;
		const float RandomY = RandomSign * FMath::RandRange(-BoxExtent.Y, BoxExtent.Y);
		SpawnLocation += FVector(RandomX, RandomY, 0.f);

		
		HiddenAnimal->SetActorLocation(SpawnLocation);
		
		HiddenAnimal->SetVisibleInGame();
	}
}
