// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "EMSActorSaveInterface.h"
#include "EmberWidgetComponent.generated.h"


UCLASS()
class PROJECTEMBER_API UEmberWidgetComponent : public UWidgetComponent, public IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	void UpdateAbilitySystemComponent(AActor* InOwner) const;
protected:
	virtual void InitWidget() override;
};
