// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "EMSActorSaveInterface.h"
#include "BaseAIAnimal.generated.h"

class UMeleeTraceComponent;
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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	
	virtual void ActorPreSave_Implementation() override;
	virtual void ActorLoaded_Implementation() override;
	virtual void PostInitializeComponents() override;

	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnFullnessChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	
	UFUNCTION(BlueprintCallable, Category = AI)
	void PlayInteractMontage(uint8 InState);

	EAnimalAIState GetCurrentState();
	EAnimalAIPersonality GetPersonality();
	float GetWildPower() const;
	float GetWanderRange() const;
	const UAnimMontage* GetMontage();
	void SetCurrentState(EAnimalAIState NewState);
	
	void GenerateRandom();
	void DecreaseFullness();
	
	
 /* AbilitySystem */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	const class UEmberAnimalAttributeSet* GetAnimalAttributeSet() const;
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	const class UEmberCharacterAttributeSet* GetCharacterAttributeSet() const;

	UFUNCTION()
	void OnHit(AActor* InstigatorActor);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeleeTraceComponent* MeleeTraceComponent;
	
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
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimalEnum)
	EAnimalAIState	CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimalEnum)
	EAnimalAIPersonality Personality;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSleep = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsHungry = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSwim = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Fullness = 40.0f; //포만감
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WildPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WanderRange = 500.0f;
	
	FTimerHandle TimerHandle;
};
