// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "EMSActorSaveInterface.h"
#include "GameplayTagAssetInterface.h"
#include "MessageBus/MessageBus.h"
#include "BaseAIAnimal.generated.h"

class UMeleeTraceComponent;
class UBoxComponent;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
enum class EAnimalAIPersonality : uint8;
class UBlackboardComponent;
class AAIAnimalController;
class UNavigationInvokerComponent;
class AAIController;

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
class PROJECTEMBER_API ABaseAIAnimal : public ACharacter, public IAbilitySystemInterface, public IEMSActorSaveInterface, public IGameplayTagAssetInterface
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
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnFullnessChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	
	EAnimalAIPersonality GetPersonality();
	float GetWildPower() const;
	float GetWanderRange() const;
	UAnimMontage* GetMontage(FGameplayTag MontageTag);
	TArray<FVector>& GetPatrolPoints();
	
	void GenerateRandom();
	void DecreaseFullness();

	UFUNCTION(BlueprintCallable)
	FGameplayTagContainer& GetGameplayTagContainer();

	UFUNCTION(BlueprintCallable, Category = AI)
	FGameplayTag GetIdentityTag() const;
	
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetIdentityTag(const FGameplayTag InIdentityTag);
	
	UFUNCTION()
	void SetHiddenInGame();

	UFUNCTION(BlueprintCallable)
	void SetVisibleInGame();
	
 /* AbilitySystem */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	const class UEmberAnimalAttributeSet* GetAnimalAttributeSet() const;
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	const class UEmberCharacterAttributeSet* GetCharacterAttributeSet() const;

	UFUNCTION()
	void OnHit(AActor* InstigatorActor);
	
protected:
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetDetails();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeleeTraceComponent* MeleeTraceComponent;
	
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
	TMap<FGameplayTag, UAnimMontage*> MontageMap; //키로 태그 넘겨주면 몽타주 가져옴 -> TSet이나 TMap 으로 바꿀 것 
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FGameplayTagContainer AnimalTagContainer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag IdentityTag;
	
	FTimerHandle TimerHandle;
	TArray<FVector> PatrolPoints;

private:
	void ReceiveMessage(const FName MessageType, UObject* Payload);
	
	FMessageDelegate MessageDelegateHandle;
};

/*
 *제일 시급한거 EQS로 수정하기 -> 장소, 타겟 찾을 때 뚝딱거림
 *동물 태어났을 때 블랙보드 정지, 죽었을 때 숨김처리, 블랙보드 리셋
 *
 *스포너 버그 수정 및 기능 추가 
 *이동 가능 범위, 무리 구역 범위, 인식 범위 실제 월드에 배치해보고 디테일하게 정해서 수정해야함
 *
 *공격 동물들 각자 공격패턴 구현
 */

