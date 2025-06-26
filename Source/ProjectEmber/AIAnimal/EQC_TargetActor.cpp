// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/EQC_TargetActor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEQC_TargetActor::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	 UObject* QueryOwner = QueryInstance.Owner.Get();
	if (!IsValid(QueryOwner))
	{
		return;
	}
	AActor* OwnerActor = Cast<AActor>(QueryOwner);
	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	AAIController* AIController = nullptr;

	if (OwnerPawn)
	{
		AIController = Cast<AAIController>(OwnerPawn->GetController());
	}

	if (!AIController)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();

	if (!BlackboardComp)
	{
		return;
	}

	UObject* TargetObject = BlackboardComp->GetValueAsObject("TargetActor");
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (TargetActor)
	{
		// 컨텍스트에 대상 액터 추가
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetActor);
	}
}
