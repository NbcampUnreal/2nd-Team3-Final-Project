// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "EMSActorSaveInterface.h"
#include "GameFramework/Character.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagAssetInterface.h"
#include "TargetSystemTargetableInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/SphereComponent.h"
#include "MessageBus/MessageBus.h"
#include "BaseAIAnimal.generated.h"

class UWidgetComponent;
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
	Normal			UMETA(DisplayName = "Normal"),			//인지 -> Idle, 피격-> 확정 도망
	Cowardly        UMETA(DisplayName = "Cowardly"),        // 인지 -> 도망 확률 증가, 피격-> 확정 도망
	Curious			UMETA(DisplayName = "Curious"),			// 인지 -> 접근 확률 증가, 피격-> 확정 도망
	Brave			UMETA(DisplayName = "Brave"),			// 인지 -> 선공격 확률 증가, 피격-> 반격 확률 증가
	Agile			UMETA(DisplayName = "Agile"),			// 기본 이동속도 증가
	Lazy			UMETA(DisplayName = "Lazy"),			// 기본 이동속도 감소
	End				UMETA(DisplayName = "End")
};

UCLASS()
class PROJECTEMBER_API ABaseAIAnimal : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface,
public ITargetSystemTargetableInterface
{
	GENERATED_BODY()

public:
	ABaseAIAnimal();

	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override; //어빌리티시스템 함수 오버라이드

	virtual bool IsTargetable_Implementation() const override;
	

	UFUNCTION()
	void OnHit(AActor* InstigatorActor);

	UFUNCTION()
	void OnAttackSpecial();
	
	UFUNCTION(BlueprintCallable)
	void OnGameTimeChanged(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
	void MakeRandomActiveAtNight(int32 InWeather);

	void OnWalkSpeedChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnMaxHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnFullnessChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	
	
	EAnimalAIPersonality GetPersonality();
	float GetWildPower() const;
	float GetWanderRange() const;
	TObjectPtr<UAnimMontage> GetMontage(FGameplayTag MontageTag);
	
	void GenerateRandom();
	void DecreaseFullness();

	UFUNCTION(BlueprintCallable, Category = AI)
	FGameplayTag GetIdentityTag() const;
	
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetIdentityTag(const FGameplayTag InIdentityTag);

	UFUNCTION(BlueprintCallable, Category = AI)
	FName GetRoleTag() const;
	
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetRoleTag(FName InRoleTag);

	UFUNCTION(BlueprintCallable)
	void SetState(bool IsShouldSleep = true);

	bool GetIsShouldSleep() const;
	void SetIsShouldSleep(bool InIsSleep);

	//밤에 활동,비활동
	UFUNCTION(BlueprintCallable)
	void ActiveNonSleep();
	
	UFUNCTION(BlueprintCallable)
	void DeactiveSleep();

	UFUNCTION(BlueprintCallable, Category = AI)
	bool GetIsDead() const;
	
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetIsDead(const bool InIsDead);

	UFUNCTION(BlueprintCallable, Category = SoundPitch)
	int32 GetSoundIndex() const;
	
	
	/* Spawn & Despawn*/
	UFUNCTION()
	void SetHiddenInGame();

	UFUNCTION(BlueprintCallable)
	void SetVisibleInGame();

	UFUNCTION()
	void OnBeginDeath();

 /* AbilitySystem */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	const class UEmberAnimalAttributeSet* GetAnimalAttributeSet() const;
	
	UFUNCTION(BlueprintCallable, Category = Attribute)
	const class UEmberCharacterAttributeSet* GetCharacterAttributeSet() const;



protected:
	void ReceiveMessage(const FName MessageType, UObject* Payload);
	void ApplyWaterSurface(float DeltaTime);
	
	UFUNCTION(BlueprintCallable, Category = AI)
	void SetDetails();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* RSphereComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* LSphereComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeleeTraceComponent* MeleeTraceComponent;
	
	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(SaveGame)
	TObjectPtr<class UEmberCharacterAttributeSet> CharacterAttributeSet;
	
	UPROPERTY(SaveGame)
	TObjectPtr<class UEmberAnimalAttributeSet> AnimalAttributeSet;
	
	UPROPERTY(EditAnywhere, Category = "HpBar")
	TSubclassOf<class UUserWidget> HpBarWidgetClass;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class UEmberWidgetComponent> HpBarWidget;
	
	// Invoker 관련 변수
	UPROPERTY(BlueprintReadWrite, Category = Navigation, meta = (AllowPrivateAccess = "true"))
	UNavigationInvokerComponent* NavInvokerComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	float NavGenerationRadius;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Navigation)
	float NavRemoveRadius;

	//
	UPROPERTY()
	AAIAnimalController* AIController;
	
	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage)
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> MontageMap; //키로 태그 넘겨주면 몽타주 가져옴 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Montage)
	TArray<TObjectPtr<UAnimMontage>> AttackMontages;
	
	UPROPERTY(EditAnywhere, Category = "12")
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimalEnum)
	EAnimalAIPersonality Personality;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSleep = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsHungry = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool bIsShouldSwim = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI") //스포너 조건에 필요한데, 스포너에서 애니멀 블랙보드 접근x 위함 
	bool bIsDead = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Fullness = 40.0f; //포만감
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WildPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float WanderRange = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundPitch")
	int32 SoundIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag IdentityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag RoleTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMaterialInterface>> MaterialInterface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UNiagaraComponent> NiagaraComponent;
	
	FTimerHandle FullnessTimerHandle;
	
	FMessageDelegate MessageDelegateHandle;

	//수영 변수 -> 과연 dx같은 이 방법이 맞는가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swim")
	float SwimTime = 0.0f; // 시간 누적용 변수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swim")
	float FloatAmplitude = 20.0f; // 진폭 (높이)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swim")
	float FloatFrequency = 0.4f; // 주기 (1초당 오르내림)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swim")
	float WaterSurfaceZ = 0.0f; // 기준 물 표면 높이
};