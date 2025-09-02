// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LoopTimeOfDaySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPauseChanged, bool, bIsPaused);
/**
 * 
 */

UCLASS()
class PROJECTEMBER_API ULoopTimeOfDaySubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "LoopTimeOfDaySubsystem")
	static ULoopTimeOfDaySubsystem* GetInstance(UObject* WorldContext);
	
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(ULoopTimeOfDaySubsystem, STATGROUP_Tickables); }
	virtual bool IsTickable() const override { return !bPause; }
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }

	UFUNCTION(BlueprintCallable, Category = "Time")
	void StartTimeLoop();
	
	UFUNCTION(BlueprintCallable, Category = "Time")
	float GetNormalizedTime() const;

	UFUNCTION(BlueprintCallable, Category = "Time")
	int GetHour();
	
	UFUNCTION(BlueprintCallable, Category = "Time")
	void SetPause(bool Val);

	UPROPERTY(BlueprintAssignable, Category = "TimeOfDay")
	FOnPauseChanged OnPauseChanged;

	int32 GetTotalSec() const {return TotalSec;}
	int32 GetStartTime() const {return StartTime;}
	int32 GetEndTime() const {return EndTime;}
	int32 GetGameTime() const {return GameSecond;}
	int32 GetGameHour() const {return GameHour;}
protected:
	bool UpdateTime(float DeltaTime);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TotalSec = 57600.0f; // 16시간 초로 변환
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 StartTime = 28800; // 8 * 3600

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 EndTime = 86400; // 24 * 3600

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 RealTime = 60;//600; // 10 * 60

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 GameSecond = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	int32 GameHour = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float CurrentTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TimeSpeed = 48.f; // 현실 1초당 48초 -> 현실 20분 되면 하루 끝

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float TickInterval = 0.f;
	
	bool bPause = true;
	bool bEvent23Called = false;
	bool bEvent24Called = false;
};
