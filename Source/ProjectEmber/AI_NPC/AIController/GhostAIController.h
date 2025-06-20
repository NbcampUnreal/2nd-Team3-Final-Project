// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GhostAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API AGhostAIController : public AAIController
{
	GENERATED_BODY()

public:
    AGhostAIController();  // 持失切 識情
    virtual void BeginPlay() override;


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class UBlackboardComponent* BlackboardComp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
    class UBehaviorTree* BehaviorTreeAsset;
};

