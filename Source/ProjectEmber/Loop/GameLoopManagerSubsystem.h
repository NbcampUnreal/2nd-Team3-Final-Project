// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLoopManagerSubsystem.generated.h"
// 루프 종료 이벤트 (하루 끝날 때 연출 시작)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoopEndingSignature);
// 루프 전환 이벤트 (루프 번호 증가 후 알림)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoopAdvancedSignature, int32, NewLoopID);
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UGameLoopManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	int32 GetCurrentLoopID() const {return CurrentLoopID;}
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void AdvanceLoop();

	UPROPERTY(BlueprintAssignable)
	FOnLoopEndingSignature OnLoopEnding;
	
	UPROPERTY(BlueprintAssignable)
	FOnLoopAdvancedSignature OnLoopAdvanced;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentLoopID = 0;

private:
	void EndLoop();
	void StartLoop();
};
