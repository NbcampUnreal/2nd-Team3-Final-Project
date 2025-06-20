// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GhostActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UGhostActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UGhostActorComponent();

protected:
    virtual void BeginPlay() override;

private:
    void GhostFollowPlayer();

    FTimerHandle GhostFollowTimerHandle;

    float TeleportThreshold = 20000000000000.f; // 순간이동 거리
    float Speed = 200.f;              // 이동 속도
    float StopThreshold = 0.1f;
    bool bIsFollowing = true;
    FVector LastPlayerLocation;


};
