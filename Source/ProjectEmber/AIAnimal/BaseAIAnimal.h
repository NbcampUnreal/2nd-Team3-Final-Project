// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseAIAnimal.generated.h"

class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
enum class EAnimalAIPersonality : uint8;
enum class EAnimalAIState : uint8;
class UBlackboardComponent;
class AAIAnimalController;
class UNavigationInvokerComponent;
class AAIController;


UENUM(BlueprintType)
enum class EAnimalAIState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	FindFood        UMETA(DisplayName = "FindFood"), 
	Wander			UMETA(DisplayName = "Wander"),
	Attack			UMETA(DisplayName = "Attack"),
	Hit				UMETA(DisplayName = "Hit"),
	Warning			UMETA(DisplayName = "Warning"),
	Rest			UMETA(DisplayName = "Rest"),
	Flee			UMETA(DisplayName = "Flee"),
	Dead			UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EAnimalAIPersonality : uint8
{
	Normal			UMETA(DisplayName = "Normal"),			//디폴트
	Cowardly        UMETA(DisplayName = "Cowardly"),        // 기본 경계거리 증가
	Curious			UMETA(DisplayName = "Curious"),			// 플레이어에 접근
	Brave			UMETA(DisplayName = "Brave"),			// 위협받으면 반격
	Outsider		UMETA(DisplayName = "Outsider"),		// 기본 어슬렁거리 증가
	Agile			UMETA(DisplayName = "Agile"),			// 기본 이동속도 증가
	Lazy			UMETA(DisplayName = "Lazy")				// 기본 이동속도 감소
};

UCLASS()
class PROJECTEMBER_API ABaseAIAnimal : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseAIAnimal();

	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = AI)
	void PlayInteractMontage(uint8 InState);
	
	float GetWarningRange() const;
	float GetWanderRange() const;
	int32 GetWildPower() const;
	EAnimalAIState GetCurrentState() const;

	void SetCurrentState(EAnimalAIState NewState);


	
protected:
	UFUNCTION(BlueprintCallable, Category = AI)
	EAnimalAIPersonality GetPersonality() const;
	
	UFUNCTION(BlueprintCallable, Category = AI)
	UNavigationInvokerComponent* GetNavInvoker() const;

	//DT 생성 전까지 쓸 Test함수
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetDetails();

	// Invoker 관련 변수
	UPROPERTY(BlueprintReadWrite, Category = Navigation, meta = (AllowPrivateAccess = "true"))
	UNavigationInvokerComponent* NavInvokerComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	float NavGenerationRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	float NavRemovalRadius;

	//
	UPROPERTY()
	AAIAnimalController* AIController;
	
	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage)
	UAnimMontage* Montage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimalEnum)
	EAnimalAIState	CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimalEnum)
	EAnimalAIPersonality Personality;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "AI")
	float BumperRange = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WarningRange = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WalkSpeed = 300;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WanderRange = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	int32 WildPower = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSleep = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsHungry = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSwim = false;
	
	//주변 동물이 맞는다면 도망

	//5.7 To Do
	/*
	 *랜던 성격 부여
	 *임시먹이 오브젝트 설치 o
	 *임시먹이 탐색 o
	 *먹이로 이동 o
	 *상호작용 - eat 애니메이션, 몇초후idle 전환, 임시먹이 오브젝트 삭제
	*/
	
	//5.8 To Do
	/*
	 *프로젝트 합치기
	 *어빌리티시스템 적용
	 *육식,초식, 단독,무리->인터페이스인지, 부모자식상속인지
	*/
	
};