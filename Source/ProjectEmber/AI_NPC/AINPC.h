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

    // 거리 체크 및 타겟 지정
    UFUNCTION()
    void CheckPlayerDistance();

    // 플레이어를 바라보는 회전
    UFUNCTION()
    void FacePlayer();

    // 타이머 핸들
    FTimerHandle DistanceCheckTimerHandle;
    FTimerHandle FacePlayerTimerHandle;

    // 컴포넌트들
    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BodyMesh;

    UPROPERTY(VisibleAnywhere)
    UCapsuleComponent* CollisionComp;
};