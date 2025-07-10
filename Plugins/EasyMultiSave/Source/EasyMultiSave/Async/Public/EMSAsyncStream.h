//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#pragma once

#include "EMSData.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EMSAsyncStream.generated.h"

class UEMSObject;

UCLASS()
class EASYMULTISAVE_API UEMSAsyncStream : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY()
	TObjectPtr<ULevel> StreamingLevel = nullptr;

	ESaveGameMode Mode;

private:

	UPROPERTY()
	TObjectPtr<UEMSObject> EMS;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> StreamActors;

	UPROPERTY(Transient)
	TMap<FName, const TWeakObjectPtr<AActor>> StreamActorsMap;

	UPROPERTY(Transient)
	FMultiLevelStreamingData PrunedData;

	uint8 bIsActive : 1;

	int32 TotalActors;
	int32 LoadedActorNum;
	int32 LoadedTotalNum;
	int32 FailSafeLoopNum;

public:

	UFUNCTION()
	static bool InitStreamingLoadTask(UEMSObject* EMSObject, ULevel* InLevel);

	virtual void Activate() override;
    FORCEINLINE bool IsActive() const { return bIsActive; }

	void ForceDestroy();

private:

	void StartLoad();
	bool SetupLevelActors();

	void LoadActor(TWeakObjectPtr<AActor> ActorWeakPtr);
	void LoadActorMultiThread(TWeakObjectPtr<AActor> ActorWeakPtr);
	void LoadStreamingActor(AActor* Actor, const FActorSaveData& ActorData);

	void FinishLoadingStreamLevel();

	void ScheduleStreamingLoad();
	void DeferredLoadStreamActors();

	void EndTask(const bool bBroadcastFinish);
	void Deactivate();
};
