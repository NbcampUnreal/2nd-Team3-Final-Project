// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "EMSActorSaveInterface.h"
#include "BaseAIAnimal.generated.h"

class UBoxComponent;
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
	Wander			UMETA(DisplayName = "Wander"),
	Attack			UMETA(DisplayName = "Attack"),
	Hit				UMETA(DisplayName = "Hit"),
	Warning			UMETA(DisplayName = "Warning"),
	Rest			UMETA(DisplayName = "Rest"),
	Flee			UMETA(DisplayName = "Flee"),
	Dead			UMETA(DisplayName = "Dead"),
	StateEnd		UMETA(DisplayName = "StateEnd")
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
	Lazy			UMETA(DisplayName = "Lazy"),			// 기본 이동속도 감소
	End				UMETA(DisplayName = "End")
};

UCLASS()
class PROJECTEMBER_API ABaseAIAnimal : public ACharacter, public IAbilitySystemInterface, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	ABaseAIAnimal();

	virtual void PossessedBy(AController* NewController) override;
	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void ActorPreSave_Implementation() override;
	virtual void ActorLoaded_Implementation() override;
	
	UFUNCTION(BlueprintCallable, Category = AI)
	void PlayInteractMontage(uint8 InState);
	
	float GetWildPower() const;
	EAnimalAIState GetCurrentState() const;

	void SetCurrentState(EAnimalAIState NewState);
	void SetFullness();

	void GenerateRandom();
	void DecreaseFullness();
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;


public: /* AbilitySystem */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	const class UEmberAnimalAttributeSet* GetAnimalAttributeSet() const;
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	const class UEmberCharacterAttributeSet* GetCharacterAttributeSet() const;

	UFUNCTION()
	void OnHit(AActor* InstigatorActor);
	
protected:
	UFUNCTION(BlueprintCallable, Category = AI)
	EAnimalAIPersonality GetPersonality() const;
	
	UFUNCTION(BlueprintCallable, Category = AI)
	UNavigationInvokerComponent* GetNavInvoker() const;

	//DT 생성 전까지 쓸 Test함수
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetDetails();
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem", SaveGame)
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(SaveGame)
	TObjectPtr<class UEmberCharacterAttributeSet> CharacterAttributeSet;
	
	UPROPERTY(SaveGame)
	TObjectPtr<class UEmberAnimalAttributeSet> AnimalAttributeSet;
	
	UPROPERTY(EditAnywhere, Category = "HpBar")
	TSubclassOf<class UUserWidget> HpBarWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, SaveGame)
	TObjectPtr<class UEmberWidgetComponent> HpBarWidget;
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSleep = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsHungry = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Fullness = 100.f; //포만감
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSwim = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WildPower;

	FTimerHandle TimerHandle;

	//콜라이더 추가해서 오버랩 이벤트 -> 누가 때렸는지(타겟엑터) -> IsHit 변경(BT를 위한), state 변경(Task를 위한)-> BT에서 성격에 따라 분기(일반이면 도망, 용감함이면 쫓아가면서 공격)
	//블랙보드에 성격 추가, BT에도 성격으로 조건 거는 부분 있음 -> 코드 작성할 때 성격 확인 해야함
	//공격적동물용 BT 따로 생성함
	//체이서용 파인드타겟 task 따로 만들기, state 변경도 분리해야함(분기에 따라 하나씩 다 만드는 것도 고려해봄직)
};


