// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EMSActorSaveInterface.h"
#include "Engine/TriggerBox.h"
#include "SmallWidgetTriggerBox.generated.h"

UCLASS()
class PROJECTEMBER_API ASmallWidgetTriggerBox : public ATriggerBox, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	ASmallWidgetTriggerBox();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> TutorialSmallWidgetClass;

	UPROPERTY()
	UUserWidget* TutorialSmallWidget;

	UPROPERTY(SaveGame)
	bool bHasTriggered = false;
};