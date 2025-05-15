// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectEmber/AIAnimal/BT/BTDecorator_HasInteractTag.h"

#include "AIController.h"
#include "AIAnimal/TestFood.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interactables/Interactable.h"

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
	if (TargetActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		if (Cast<ATestFood>(TargetActor)->GetGameplayTagContainer().HasTag(FoodTag))
		{
			return true; 
		}
	}
	return false;
}
