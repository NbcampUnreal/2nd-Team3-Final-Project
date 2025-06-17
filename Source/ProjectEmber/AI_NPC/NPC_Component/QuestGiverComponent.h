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

    /** 느낌표 표시용 Static Mesh */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestGiver")
    UStaticMesh* ExclamationMarkMesh;

    /** 실제로 머리 위에 붙을 Mesh Component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuestGiver")
    UStaticMeshComponent* ExclamationMarkComponent;

    /** 느낌표 생성 및 붙이기 */
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void InitializeExclamationMark();

    /** 표시 켜기 */
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void ShowExclamationMark();

    /** 표시 끄기 */
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void HideExclamationMark();
};