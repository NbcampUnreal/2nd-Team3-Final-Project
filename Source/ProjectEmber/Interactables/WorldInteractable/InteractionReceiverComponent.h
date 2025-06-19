// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionReceiverComponent.generated.h"

class UInteractionCondition;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction1, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction2, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction3, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction4, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction5, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction6, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction7, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction8, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction9, AActor*, CausingActor);  
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivateAction10, AActor*, CausingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionCompleted, AActor*, CompletedBy, bool, bCanBeTriggeredAgain);


UCLASS( Blueprintable, meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UInteractionReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionReceiverComponent();

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void EvaluateDeactivationConditions(const TArray<UInteractionCondition*>& Conditions);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void BroadCastInteractionCompleted(AActor* CompletedBy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Interaction")
	bool bCanBeTriggeredAgain = true;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FOnInteractionCompleted OnInteractionCompleted;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction1 ActivateAction1;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction2 ActivateAction2;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction3 ActivateAction3;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction4 ActivateAction4;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction5 ActivateAction5;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction6 ActivateAction6;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction7 ActivateAction7;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction8 ActivateAction8;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction9 ActivateAction9;
	
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FActivateAction10 ActivateAction10;
};
