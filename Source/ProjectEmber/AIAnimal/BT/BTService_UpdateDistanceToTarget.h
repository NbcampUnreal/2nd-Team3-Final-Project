// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_UpdateDistanceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTService_UpdateDistanceToTarget : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_UpdateDistanceToTarget();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
