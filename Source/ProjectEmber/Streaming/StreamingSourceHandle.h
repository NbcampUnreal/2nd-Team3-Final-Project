// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StreamingSourceHandle.generated.h"

class UStreamingSourceProvider;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStreamingCompleted);

/**
 * 스트리밍 소스 편의성 추가한 버전
 * BP에서도 쓰기 쉽게 설계
 * Initialize에서 스트리밍할 구역 넣어주고
 * BeginWait 시작
 * 스트리밍 완료하면 델리게이트 호출
 * 다 쓰면 Cleanup 호출하기!!!!!!
 */
UCLASS(BlueprintType)
class PROJECTEMBER_API UStreamingSourceHandle : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Streaming")
	void Initialize(const TArray<FVector>& InLocations);

	UFUNCTION(BlueprintCallable, Category="Streaming")
	void BeginWait();

	UFUNCTION(BlueprintCallable, Category="Streaming")
	void Cleanup();

	UPROPERTY(BlueprintAssignable, Category="Streaming")
	FOnStreamingCompleted OnStreamingCompleted;

private:
	void OnCheckStreaming();
	
	UPROPERTY()
	TObjectPtr<UStreamingSourceProvider> StreamingSourceProvider;

	UPROPERTY()
	UWorldPartitionSubsystem* Subsystem;

	UPROPERTY()
	TArray<FVector> Locations;
};
