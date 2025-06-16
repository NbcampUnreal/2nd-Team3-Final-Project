// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTService_RunBestEQS.h"

#include "AIController.h"
#include "AIAnimal/TokenRaidSubsystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"

UBTService_RunBestEQS::UBTService_RunBestEQS()
{
}

void UBTService_RunBestEQS::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
}

void UBTService_RunBestEQS::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return;
	}

	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	Cast<AAIAnimalController>(Cast<ABaseAIAnimal>(AIPawn)->GetController())->GetBlackboardComponent()->SetValueAsVector("TargetLocation", Player->GetActorLocation());

	if (Cast<AAIAnimalController>(Cast<ABaseAIAnimal>(AIPawn)->GetController())->GetBlackboardComponent()->GetValueAsName("NGroupTag") != "Animal.Role.Leader")
	{
		return;
	}
	
	//EQS
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
	 GetWorld(),
	BestPointQuery,
	AIPawn,
	 EEnvQueryRunMode::AllMatching,
	 nullptr);
	
	if (QueryInstance)
	{
		if (UTokenRaidSubsystem* TokenRaidSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTokenRaidSubsystem>())
		{
			if (QueryInstance)
			{
				QueryInstance->GetOnQueryFinishedEvent().AddDynamic(TokenRaidSubsystem, &UTokenRaidSubsystem::OnEQSComplete);
			}
		}
	}
}

void UBTService_RunBestEQS::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
}
