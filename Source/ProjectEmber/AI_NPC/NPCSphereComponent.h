// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "NPCSphereComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTEMBER_API UNPCSphereComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UNPCSphereComponent();
};
