#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "AbilityInteractBox.generated.h"

UCLASS()
class PROJECTEMBER_API AAbilityInteractBox : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAbilityInteractBox();
protected:
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void PostInitializeComponents() override;
	
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	void ApplyEffectToTarget(AActor* TargetActor);
	void InvokeGameplayCue(AActor* TargetActor);
	
protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, Category = "Box")
	TObjectPtr<class UBoxComponent> Trigger;

	UPROPERTY(VisibleAnywhere, Category = "Box")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem")
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "AbilitySystem", meta=(Categories=GameplayCue))
	FGameplayTag GameplayCueTag;
};
