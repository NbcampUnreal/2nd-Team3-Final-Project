// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectEmber/AIAnimal/BTDecorator_HasInteractTag.h"

#include "AIController.h"
#include "TestFood.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_HasInteractTag::UBTDecorator_HasInteractTag()
{
	NodeName = TEXT("HasInteractTag");
}

bool UBTDecorator_HasInteractTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return false;
	}

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return false;
	}
	UObject* TargetObject = BlackboardComp->GetValueAsObject("TargetActor");
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetObject)
	{
		return false;
	}
	if (TargetActor->GetClass()->ImplementsInterface(UInteractiveObject::StaticClass()))
	{
		if (IInteractiveObject::Execute_GetTag(TargetActor).MatchesTag(FoodTag))
		{
			return true; 
		}
	}
	return false;
}
