// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AINPC.generated.h"

class UPawnSensingComponent;

UCLASS()
class PROJECTEMBER_API AAINPC : public ACharacter
{
	GENERATED_BODY()

public:
    AAINPC();

protected:
    virtual void BeginPlay() override;

    // �Ÿ� üũ �� Ÿ�� ����
    UFUNCTION()
    void CheckPlayerDistance();

    // �÷��̾ �ٶ󺸴� ȸ��
    UFUNCTION()
    void FacePlayer();

    // Ÿ�̸� �ڵ�
    FTimerHandle DistanceCheckTimerHandle;
    FTimerHandle FacePlayerTimerHandle;

    // ������Ʈ��
    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BodyMesh;

    UPROPERTY(VisibleAnywhere)
    UCapsuleComponent* CollisionComp;
};