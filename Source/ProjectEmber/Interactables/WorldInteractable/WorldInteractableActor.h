// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Condition/IInteractionConditionHandler.h"
#include "Interactables/BaseInteractableActor.h"
#include "EMSActorSaveInterface.h"
#include "WorldInteractableActor.generated.h"


class UInteractionCondition;
class UInteractionReceiverComponent;
class UQuestCondition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionStarted, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionEnded, AActor*, Interactor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivationConditionsMet, const FGameplayEventData&, EventData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAllClearConditionsMet, const FGameplayEventData&, EventData);

UCLASS()
class PROJECTEMBER_API AWorldInteractableActor : public AActor, public IInteractable, public IEMSActorSaveInterface
{
	GENERATED_BODY()
public:
	AWorldInteractableActor();
	
	/** 이벤트 태그 받을 때 쓸 함수
	 * bIsPlaying 이 true, false일 때 각각 함수 호출
	 * true면 DeactiveConditions 배열 조건 검사 후 모두 충족하면 델리게이트 브로드캐스트
	 * false면 InteractionConditions 배열 조건 검사 후 모두 충족하면 델리게이트 브로드캐스트
	 * @param EventTag 
	 * @param EventData 
	 */
	UFUNCTION(BlueprintCallable, Category = "Tags")
	virtual void HandleEventTag(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "Tags")
	virtual void HandleStartConditionEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

	UFUNCTION(BlueprintCallable, Category = "Tags")
	virtual void HandleClearConditionEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	virtual void TryInteract_Implementation(AActor* Interactor) override;
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	bool CanInteract();
	
	UFUNCTION()
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	UFUNCTION()
	virtual void EndInteract_Implementation() override;

	/**상속받은 블루프린트에서 상호작용 전처리할 때 호출*/
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionStarted OnInteractionStarted;

	/**상속받은 블루프린트에서 상호작용 후처리할 때 호출*/
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionEnded OnInteractionEnded;

	/** 모든 상호작용 가능 조건 충족시 호출*/
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnActivationConditionsMet OnAllActivationConditionsMet;

	/**모든 비활성화 조건 충족시 호출*/
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnAllClearConditionsMet OnAllClearConditionsMet;
	
protected:
	bool AreAllConditionsMet(TArray<TObjectPtr<UInteractionCondition>>& Conditions);
	
	/**상호작용을 시작할 때 필수적으로 갖춰야하는 컨디션들
	 * 태그 컨테이너 형태이고, bIsPlaying이 false면 태그가 들어올 때 이 컨디션들 검사
	 */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interaction", SaveGame)
	TArray<TObjectPtr<UInteractionCondition>> StartConditions;

	/**상호작용 완전 비활성화 조건
	 * 상호작용 도중에 태그가 들어오면 이 컨디션들을 검사
	 * 모든 컨디션들이 충족하면 종료 후 상호작용 비활성화
	 */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite, Category="Interaction", SaveGame)
	TArray<TObjectPtr<UInteractionCondition>> ClearConditions;

	UPROPERTY(BlueprintReadWrite, Category="Interaction", SaveGame)
	bool bIsPlaying = false; // 상호작용 진행 중인 상태? (TryInteract에서 갱신)
};

