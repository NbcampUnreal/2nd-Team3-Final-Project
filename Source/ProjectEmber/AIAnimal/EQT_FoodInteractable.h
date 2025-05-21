// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EQT_FoodInteractable.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEQT_FoodInteractable : public UEnvQueryTest
{
	GENERATED_BODY()
	
public:
	UEQT_FoodInteractable();

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override; //EQS가 쿼리 실행 시 호출하는 핵심 함수
};
