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

    // 거리 체크 및 타겟 지정
    UFUNCTION()
    void CheckPlayerDistance();

    // 타이머 핸들
    FTimerHandle DistanceCheckTimerHandle;
    // 컴포넌트들
    UPROPERTY(VisibleAnywhere)
    USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BodyMesh;

  
};
