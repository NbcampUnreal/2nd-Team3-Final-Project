#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction")
	TScriptInterface<IInteractable> CurrentInteractable;

	UFUNCTION()
	void OnGatherOverlapBegin(UPrimitiveComponent* OverlappedComp,
						AActor* OtherActor,
						UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex,
						bool bFromSweep,
						const FHitResult& SweepResult);

	UFUNCTION()
	void OnGatherOverlapEnd(UPrimitiveComponent* OverlappedComp,
					  AActor* OtherActor,
					  UPrimitiveComponent* OtherComp,
					  int32 OtherBodyIndex);

	
	UFUNCTION()
	void OnInteractOverlapBegin(UPrimitiveComponent* OverlappedComp,
	                            AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp,
	                            int32 OtherBodyIndex,
	                            bool bFromSweep,
	                            const FHitResult& SweepResult);

	UFUNCTION()
	void OnInteractOverlapEnd(UPrimitiveComponent* OverlappedComp,
	                          AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp,
	                          int32 OtherBodyIndex);


	// 상호작용 가능한 액터 오버랩 이벤트 발생시 인터페이스 함수 구현부 저장
	UFUNCTION()
	void SetCurrentInteractable(UObject* NewInteractable);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Interact();

	void StartGather();
	void StopGather();
	void GatherTick();


	UFUNCTION(BlueprintCallable, Category="Interaction")
	void PlayInteractMontage();

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void StopInteractMontage();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void TriggerAdvanceDialogue();
private:
	bool bIsLocked = false;

	UPROPERTY(VisibleAnywhere, Category = "Interactable")
	UBoxComponent* GatherTrigger;

	UPROPERTY(VisibleAnywhere, Category = "Interactable")
	UBoxComponent* OnInteractOverTrigger;
	
	FTimerHandle GatherTimerHandle;
	
	float GatherTime;
	float GatherElapsed;
	float GatherTickInterval = 0.02f;

	UPROPERTY()
	UAnimInstance* CachedAnimInstance;

	UPROPERTY(EditAnywhere, Category="Interaction")
	UAnimMontage* InteractMontage;

	UPROPERTY(VisibleAnywhere, Category="Animation")
	bool bLoopMontage = false;
};
