// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Tutorial/Widget/TutorialWidget.h"
#include "Engine/TriggerBox.h"
#include "Tutorial/DataAssest/TutorialDataAsset.h"
#include "TutorialBoxTrigger.generated.h"

class UBoxComponent;

UCLASS()
class PROJECTEMBER_API ATutorialBoxTrigger : public ATriggerBox
{
	GENERATED_BODY()
	

public:
    ATutorialBoxTrigger();

protected:

    UPROPERTY(VisibleAnywhere)
    UBoxComponent* BoxComponent;

public:
    //  �迭 ����: DataListAsset + Index!
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
    UTutorialDataAsset* TutorialDataAsset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
    int32 TutorialIndex = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tutorial")
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