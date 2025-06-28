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

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
    bool bHasInteractedWithPlayer = false;

protected:
    virtual void BeginPlay() override;

    // �Ÿ� üũ �� Ÿ�� ����
    UFUNCTION()
    void CheckPlayerDistance();

    // Ÿ�̸� �ڵ�
    FTimerHandle DistanceCheckTimerHandle;
    // ������Ʈ��
    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BodyMesh;

 

  
};
