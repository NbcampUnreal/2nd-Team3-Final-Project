// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EMSActorSaveInterface.h"
#include "Engine/TriggerBox.h"
#include "Tutorial/DataAssest/TutorialDataAsset.h"
#include "TutorialBoxTrigger.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTEMBER_API ATutorialBoxTrigger : public ATriggerBox, public IEMSActorSaveInterface
{
	GENERATED_BODY()
	

public:
    ATutorialBoxTrigger();

protected:

    // 게임 시작 시 호출
    virtual void BeginPlay() override;
    UPROPERTY(VisibleAnywhere)
    UBoxComponent* BoxComponent;

public:
    //  �迭 ����: DataListAsset + Index!
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
    UTutorialDataAsset* TutorialDataAsset;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<int32> TutorialIndexs; 

    UPROPERTY(SaveGame)
    bool bTriggered = false;

private:
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult);
};