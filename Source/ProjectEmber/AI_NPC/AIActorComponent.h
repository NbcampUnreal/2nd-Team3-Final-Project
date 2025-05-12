// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIActorComponent.generated.h"


class UPawnSensingComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UAIActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UAIActorComponent();

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

  
};
