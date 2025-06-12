// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MiniGameCondition.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class PROJECTEMBER_API UMiniGameCondition : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * 태그로 검사 ( 왠만하면 모든 처리를 태그화 해서 처리 )
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EventTag{FGameplayTag()};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ConditionName;
	/**
	 * 조건 충족을 위한 횟수
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 RequiredCount = 1;


	/**
	 * 런타임 카운트
	 */
	UPROPERTY(VisibleInstanceOnly)
	int32 CurrentCount = 0;

	/**
	 * 블루프린트에서도 상속 가능하게 처리
	 * 태그가 일치하면 카운트 증가 후 조건 충족 여부 확인
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnEvent(const FGameplayTag& InEventTag, const FGameplayEventData& EventData);
	virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
	{
		if (InEventTag != EventTag)
		{
			return false;
		}
		CurrentCount++;
		return IsFulfilled();
	}

	/**
	 * 조건 충족 여부 확인
	 */
	UFUNCTION(BlueprintCallable)
	bool IsFulfilled() const
	{
		return CurrentCount >= RequiredCount;
	}

	/**
	 * 반복 퀘스트 고려 리셋함수
	 */
	UFUNCTION(BlueprintCallable)
	void Reset()
	{
		CurrentCount = 0;
	}

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FString GetConditionText() const;
	virtual FString GetConditionText_Implementation() const
	{
		return FString::Printf(TEXT("진행도: %d / %d"), CurrentCount, RequiredCount);
	}

 
};
