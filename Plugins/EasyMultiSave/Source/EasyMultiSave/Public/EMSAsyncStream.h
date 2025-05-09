//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#pragma once

#include "CoreMinimal.h"
#include "EMSData.h"
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
	const ULevel* StreamingLevel = nullptr;

	ESaveGameMode Mode;

private:

	UPROPERTY()
	TObjectPtr<UEMSObject> EMS;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> StreamActors;

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
	static bool InitStreamingLoadTask(UEMSObject* EMSObject, const ULevel* InLevel);

	virtual void Activate() override;
    FORCEINLINE bool IsActive() const { return bIsActive; }

	void ForceDestroy();

private:

	void StartLoad();
	bool SetupLevelActors();

	void LoadActor(AActor* Actor);
	void LoadActorMultiThread(AActor* Actor);
	void LoadStreamingActor(AActor* Actor, const FActorSaveData& ActorData);

	void FinishLoadingStreamLevel();

	void ScheduleStreamingLoad();
	void DeferredLoadStreamActors();

	void EndTask(const bool bBroadcastFinish);
	void Deactivate();
};
