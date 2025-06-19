// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestGiverComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UQuestGiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UQuestGiverComponent();

    // === Exclamation ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestGiver")
    UStaticMesh* ExclamationMarkMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuestGiver")
    UStaticMeshComponent* ExclamationMarkComponent;

    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void InitializeExclamationMark();
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void ShowExclamationMark();
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void HideExclamationMark();

    // === Question ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestGiver")
    UStaticMesh* QuestionMarkMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "QuestGiver")
    UStaticMeshComponent* QuestionMarkComponent;

    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void InitializeQuestionMark();
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void ShowQuestionMark();
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void HideQuestionMark();

    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void HideAllMarks();

protected:
    /*추가: Attach 대상 선택 함수 */
    USceneComponent* GetAttachTarget() const;

    virtual void BeginPlay() override;
};