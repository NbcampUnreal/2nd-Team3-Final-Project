// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindPoint.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UBTTask_FindPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindPoint();

	//태스크 실행 로직
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	float SearchRadius = 3000.f;

	UPROPERTY(EditAnywhere, Category = "AI") //가까워졌을 때 완료로 판단할 거리
	float AcceptableDistance = 10.f;

	// 상호작용 가능한 오브젝트의 태그
	UPROPERTY(EditAnywhere, Category = "Search", meta = (AllowPrivateAccess = "true"))
	FName InteractiveObjectTag = "Interactive";
};
