// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EMSActorSaveInterface.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "AIAnimalController.generated.h"

namespace EEnvQueryStatus
{
	enum Type : int;
}

class UEnvQueryInstanceBlueprintWrapper;
class UEnvQuery;
enum class EAnimalAIPersonality : uint8;
class UBehaviorTreeComponent;
class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API AAIAnimalController : public AAIController, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	AAIAnimalController();

	UPROPERTY(BlueprintReadWrite, Category = "AI", SaveGame)
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(BlueprintReadWrite, Category = "AI", SaveGame)
	UBlackboardComponent* BlackboardComponent;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

	FVector SafePoint;
	
	//인식 관련 변수
	UPROPERTY(BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"), SaveGame)
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY(BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"), SaveGame)
	UAISenseConfig_Sight* SightConfig;
 
	UPROPERTY(BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"), SaveGame)
	UAISenseConfig_Hearing* HearingConfig;
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem", SaveGame)
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	//움직이는 대상 -> 플레이어, 다른 종족은 시각, 청각으로 탐지
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	void FindTargetPlayer(AActor* Actor, FAIStimulus Stimulus);
	void FindTargetAnimal(AActor* Actor, FAIStimulus Stimulus);
	void SenseInteractionWithUI(const FAIStimulus& Stimulus);
};
