// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIController.h"
#include "Blueprint/UserWidget.h"
#include "QuestReceiverComponent.h" 
#include "QuestGiverComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UQuestGiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UQuestGiverComponent();
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    void SetupComponentDispatchers(AActor* NPCRef, UQuestReceiverComponent* QuestReceiver);

    UFUNCTION(BlueprintCallable)
    void UpdateQuestMarker();

    UFUNCTION(BlueprintCallable)
    void SetupDebugBindings();

    // Callback Functions
    UFUNCTION()
    void OnQuestAccepted(const FQuestStorageInfo& QuestInfo);

    UFUNCTION()
    void OnQuestAbandoned(const FQuestStorageInfo& QuestInfo);

    UFUNCTION()
    void OnQuestCompleted(const FQuestStorageInfo& QuestInfo);

    UFUNCTION()
    void OnQuestUpdated(const FQuestStorageInfo& QuestInfo);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Visual")
    UStaticMeshComponent* ExclamationMarkSM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Visual")
    UStaticMeshComponent* QuestionMarkSM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|UI")
    TSubclassOf<UUserWidget> QuestWidgetClass;

    UPROPERTY()
    UUserWidget* QuestWidget;

    UPROPERTY()
    AAIController* CachedAIController;

    UPROPERTY()
    UQuestReceiverComponent* CachedReceiver;
};
