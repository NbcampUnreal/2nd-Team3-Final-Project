// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/BT/BTTask_AttackTarget.h"

UBTTask_AttackTarget::UBTTask_AttackTarget()
{
	NodeName = TEXT("AttackTarget");
}

EBTNodeResult::Type UBTTask_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{


	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
