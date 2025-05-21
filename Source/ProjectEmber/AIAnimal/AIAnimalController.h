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

	void InitBlackboard();
	void FindTargetPlayer(AActor* Actor, FAIStimulus Stimulus);
	void FindTargetAnimal(AActor* Actor, FAIStimulus Stimulus);
	void SenseInteractionWithUI(const FAIStimulus& Stimulus);
	void TimeOutClearFocus();

	//이 변수는 동물이 생성될 때, 밤이 남은 시간을 받와와서 초기화, 고정시간 박으면 안됨, 생성됐는데 밤 1초 남을 수 있음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float fSleepTime = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSleep = false;

	FTimerHandle FocusTimerHandle;

	//sleep 노드 관련
	static const FName SleepTime;
	static const FName IsShouldSleep;
	
	//SearchTarget 노드 관련
	static const FName DistanceToTarget;
	static const FName CurrentState;
	static const FName TargetActor;
	static const FName TargetLocation;
	static const FName FleeRange;
	static const FName WanderRange;
	static const FName IsWarning;

	//SearchTarget 노드 관련
	static const FName SafeLocation;
	static const FName IsRest;
	static const FName IsHit;
	
};
