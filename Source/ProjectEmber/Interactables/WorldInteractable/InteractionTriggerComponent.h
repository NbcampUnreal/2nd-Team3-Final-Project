// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionTypes.h"

#include "InteractionTriggerComponent.generated.h"


UCLASS( Blueprintable,  meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UInteractionTriggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionTriggerComponent();
	
	UFUNCTION(BlueprintCallable, Category="Interaction")
	void ActivateInteractions(); //등록된 액터들의 액션 실행

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void AddInteractionTarget(FName InstanceName, TEnumAsByte<EInteractionAction::Type> InteractionAction); //월드에 배치된 액터 등록. 인스턴스 이름으로 찾고 실행할 액션은 enum으로 결정
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	TArray<FInteractionTarget> InteractionTargets; // 자신에게 등록된 액터들
};
