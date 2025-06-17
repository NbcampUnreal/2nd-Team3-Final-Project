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

    /** ����ǥ ǥ�ÿ� Static Mesh */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestGiver")
    UStaticMesh* ExclamationMarkMesh;

    /** ������ �Ӹ� ���� ���� Mesh Component */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuestGiver")
    UStaticMeshComponent* ExclamationMarkComponent;

    /** ����ǥ ���� �� ���̱� */
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void InitializeExclamationMark();

    /** ǥ�� �ѱ� */
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void ShowExclamationMark();

    /** ǥ�� ���� */
    UFUNCTION(BlueprintCallable, Category = "QuestGiver")
    void HideExclamationMark();
};