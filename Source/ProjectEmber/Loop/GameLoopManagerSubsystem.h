// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameLoopManagerSubsystem.generated.h"

class ULoopEventDirector;
class ILoopEventListener;

/**
 * 
 */
UENUM(BlueprintType)
enum class ELoopEventPhase : uint8
{
	Begin UMETA(DisplayName = "Begin Loop"),
	Start UMETA(DispayName = "Start Event"),
	Mid UMETA(DispayName = "Mid"),
	End  UMETA(DispayName = "End"),
};

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
	void AdvanceLoopEvent();

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void MidLoopEvent();
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void EndLoopEvent();

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void CheckAllEndConditionReady();

	void OnDayEnd24();
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void NotifyLoopEndReady();

	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void NotifyLoopMidReady();
	
	UFUNCTION(BlueprintCallable, Category = "GameLoop")
	void NotifyLoopStartReady();
	
	TObjectPtr<ULoopEventDirector> GetEventDirector() {return LoopDirector;}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameLoop")
	float FadeDuration = 5.f;
private:
	
	void OnWorldReady(UWorld* World, const UWorld::InitializationValues IVS);
	
	UFUNCTION()
	void OnAllEndLoopReady();

	UFUNCTION()
	void OnAllMidLoopReady();

	UFUNCTION()
	void OnAllStartLoopReady();
	
	void StartFade(bool bFadeOut, const FLinearColor& Color, float Duration);

	UPROPERTY()
	int32 CurrentLoopID = 0;

	UPROPERTY()
	TObjectPtr<ULoopEventDirector> LoopDirector;

	FTimerHandle FadeTimerHandle;

	ELoopEventPhase CurrentPhase = ELoopEventPhase::Begin;

	bool bAllEndEventFinished = false;
	bool bDayTimeEnded = false;
};
