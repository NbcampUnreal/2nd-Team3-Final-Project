//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#pragma once

#include "EMSObjectBase.h"
#include "EMSData.h"
#include "EMSCustomSaveGame.h"
#include "EMSInfoSaveGame.h"
#include "EMSPluginSettings.h"
#include "EMSAsyncLoadGame.h"
#include "EMSAsyncSaveGame.h"
#include "EMSActorSaveInterface.h"
#include "EMSCompSaveInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CoreMinimal.h"
#include "Engine/Level.h"
#include "EMSObject.generated.h"

class FBufferArchive;
class FMemoryReader;
class APawn;
class APlayerController;
class UActorComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmsLoadPlayerComplete, const APlayerController*, LoadedPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmsLoadLevelComplete, const TArray<TSoftObjectPtr<AActor>>&, LoadedActors);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmsLoadPartitionComplete, const TArray<TSoftObjectPtr<AActor>>&, LoadedActors);

UCLASS(BlueprintType, meta = (DisplayName = "Easy Multi Save", Keywords = "Save, EMS, EasyMultiSave, EasySave"))
class EASYMULTISAVE_API UEMSObject : public UEMSObjectBase
{
	GENERATED_BODY()

protected:
	UEMSObject();

/** Variables */

public:

	UPROPERTY(BlueprintAssignable, Category = "Easy Multi Save | Delegates")
	FEmsLoadPlayerComplete OnPlayerLoaded;

	UPROPERTY(BlueprintAssignable, Category = "Easy Multi Save | Delegates")
	FEmsLoadLevelComplete OnLevelLoaded;

	UPROPERTY(BlueprintAssignable, Category = "Easy Multi Save | Delegates")
	FEmsLoadPartitionComplete OnPartitionLoaded;  

private:

	FCriticalSection SaveActorsScope;
	FCriticalSection LoadActorScope;

	uint8 bInitWorldPartitionLoad : 1;
	uint8 bLoadPartition : 1;
	uint8 bSavePartition : 1;

	float WorldPartitionInitTimer;

	FDelegateHandle ActorDestroyedDelegate;

private:

	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<AActor>> ActorList;

	UPROPERTY(Transient)
	TMap<FName, const TWeakObjectPtr<AActor>> ActorMap;

	UPROPERTY(Transient)
	TArray<FLevelArchive> LevelArchiveList;	

	UPROPERTY(Transient)
	FMultiLevelStreamingData MultiLevelStreamData;

	UPROPERTY(Transient)
	FPlayerStackArchive PlayerStackData;
	
	UPROPERTY(Transient)
	TArray<FActorSaveData> SavedActors;

	UPROPERTY(Transient)
	TArray<FActorSaveData> SavedActorsPruned;

	UPROPERTY(Transient)
	TArray<FLevelScriptSaveData> SavedScripts;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedGameMode;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedGameState;

	UPROPERTY(Transient)
	FControllerSaveData SavedController;

	UPROPERTY(Transient)
	FPawnSaveData SavedPawn;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedPlayerState;

	UPROPERTY(Transient)
	TSet<FActorSaveData> WorldPartitionActors;

	UPROPERTY(Transient)
	TSet<FActorSaveData> DestroyedActors;

	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<AActor>, FGameObjectSaveData> RawObjectData;

	UPROPERTY(Transient)
	TArray<TSoftObjectPtr<AActor>> RealLoadedActors;

/** Blueprint Library function accessors */
	
public:

	UObject* LoadRawObject(AActor* Actor, const FRawObjectSaveData& Data);
	bool SaveRawObject(AActor* Actor, const FRawObjectSaveData& Data);

	bool SavePlayerActorsCustom(AController* Controller, const FString& FileName);
	bool LoadPlayerActorsCustom(AController* Controller, const FString& FileName);
	bool DeleteCustomPlayerFile(const FString& FileName);

/** Other public Functions  */

public:

	static UEMSObject* Get(const UObject* WorldContextObject);

	void OnAnyActorDestroyed(AActor* DestroyedActor);

	void PrepareLoadAndSaveActors(const uint32 Flags, const EAsyncCheckType FunctionType, const EPrepareType PrepareType);

	bool SavePlayerActors(APlayerController* Controller, const FString& FileName);
	void LoadPlayerActors(APlayerController* Controller);

	bool SaveLevelActors(const bool bMemoryOnly);
	void LoadLevelActors(UEMSAsyncLoadGame* LoadTask);
	void LoadGameMode();
	void LoadLevelScripts();
	void PrepareLevelActors();	

	void SpawnOrUpdateLevelActor(const FActorSaveData& ActorArray);
	void FinishLoadingLevel(const bool bHasLoadedFile);

	bool TryLoadPlayerFile();
	bool TryLoadLevelFile();

	APlayerController* GetPlayerController() const;
	APawn* GetPlayerPawn(const APlayerController* PC) const;

	FTimerManager& GetTimerManager() const;

	bool HasValidGameMode() const;
	bool HasValidPlayer() const;

/** Internal Functions  */

protected:

	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

private:

	void SaveActorToBinary(AActor* Actor, FGameObjectSaveData& OutData) const;
	void LoadActorFromBinary(AActor* Actor, const FGameObjectSaveData& InData);

	void LoadAllLevelActors(const TWeakObjectPtr<UEMSAsyncLoadGame>& InTask);

	EUpdateActorResult UpdateLevelActor(const FActorSaveData& ActorArray);
	void SpawnLevelActor(const FActorSaveData& ActorArray);
	void ProcessLevelActor(AActor* Actor, const FActorSaveData& ActorArray);
	void CreateLevelActor(UClass* SpawnClass, const FActorSaveData& ActorArray, const FActorSpawnParameters& SpawnParams);
	void FailSpawnLevelActor(const FActorSaveData& ActorArray) const;

	bool UnpackBinaryArchive(const EDataLoadType LoadType, FMemoryReader& FromBinary, UObject* Object = nullptr) override;
	bool UnpackLevelArchive(FMemoryReader& FromBinary);
	bool UnpackPlayerArchive(FMemoryReader& FromBinary);
	bool UnpackLevel(const FLevelArchive& LevelArchive);
	void UnpackPlayer(const FPlayerArchive& PlayerArchive);

	void PrepareLoadActor(const uint32 Flags, AActor* Actor, const bool bFullReload);
	void PrepareSaveActor(const EPrepareType PrepareType, AActor* Actor);
	void PrepareFullReload(const uint32 Flags, AActor* Actor) const;
	void AddActorToList(AActor* Actor, const bool bIsLoading);
	
	void DirectSetPlayerPosition(const FPlayerPositionArchive& PosArchive);
	
	FGameObjectSaveData ParseGameModeObjectForSaving(AActor* Actor) const;
	FLevelScriptSaveData ParseLevelScriptForSaving(AActor* Actor) const;
	FActorSaveData ParseLevelActorForSaving(AActor* Actor, const EActorType Type) const;

	void ExecuteActorPreSave(AActor* Actor) const;
	void ExecuteActorSaved(AActor* Actor) const;
	void ExecuteActorPreLoad(AActor* Actor) const;
	void ExecuteActorLoaded(AActor* Actor) const;

	void SerializeActorStructProperties(AActor* Actor) const;

	TArray<UActorComponent*> GetSaveComponents(AActor* Actor) const;
	void SaveActorComponents(AActor* Actor, TArray<FComponentSaveData>& OutComponents) const;
	void LoadActorComponents(AActor* Actor, const TArray<FComponentSaveData>& InComponents);

	UObject* SerializeFromRawObject(AActor* Actor, const FRawObjectSaveData& Data, const TArray<FComponentSaveData>& InputArray);
	void AppendRawObjectData(AActor* Actor, TArray<FComponentSaveData>& OutComponents) const;
	void UpdateRawObjectData(AActor* Actor, const FComponentSaveData& InputData);

	FLevelStackArchive AddMultiLevelStackData(const FLevelArchive& LevelArchive, const FLevelArchive& PersistentArchive, const FGameObjectSaveData& InGameMode, const FGameObjectSaveData& InGameState);
	FLevelArchive AddMultiLevelStreamData(const FLevelArchive& LevelArchive);
	void UpdateMultiLevelStreamData(const FLevelArchive& LevelArchive);

/** World Partition Functions  */

private:

	void OnLevelStreamingStateChanged(UWorld* InWorld, const ULevelStreaming* InStreamingLevel, ULevel* InLevelIfLoaded, ELevelStreamingState PreviousState, ELevelStreamingState NewState);
	void OnLevelBeginMakingInvisible(UWorld* InWorld, const ULevelStreaming* InStreamingLevel, ULevel* InLoadedLevel);

	void OnPreWorldInit(UWorld* World, const UWorld::InitializationValues IVS);
	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);

	void RemoveWorldPartitionStreamDelegates();
	
	void TryInitWorldPartition();
	void PollInitWorldPartition();

	void TrySaveWorldPartition();
	void AccumulatedSaveWorldPartition();

public:

	void LoadStreamingActor(AActor* Actor, const FActorSaveData& ActorData);

/** Delegate Functions  */

public:

	FORCEINLINE void BroadcastOnPartitionLoaded()
	{
		OnPartitionLoaded.Broadcast(RealLoadedActors);
		RealLoadedActors.Empty();
	}

	FORCEINLINE void BroadcastOnLevelLoaded()
	{
		OnLevelLoaded.Broadcast(RealLoadedActors);
		RealLoadedActors.Empty();
	}

	FORCEINLINE void AllocateRealLoadedActors(const int32 AllocNum)
	{
		RealLoadedActors.Reserve(AllocNum);
	}

	FORCEINLINE bool HasActuallyLoadedActors()
	{
		return !EMS::ArrayEmpty(RealLoadedActors);
	}

/** Clear Data Functions  */

public:

	FORCEINLINE void ClearMultiLevelSave()
	{
		ClearSavedLevelActors();
		ClearStreamingData();
		LevelArchiveList.Empty();
		PlayerStackData = FPlayerStackArchive();
		bLoadFromMemory = false;
	}

	FORCEINLINE void ClearWorldPartition()
	{
		WorldPartitionActors.Empty();
		bSavePartition = false;
		bLoadPartition = false;
		bInitWorldPartitionLoad = false;
		bLoadFromMemory = false;
		WorldPartitionInitTimer = 0.f;
	}

private:

	FORCEINLINE void ClearPlayerPosition()
	{
		SavedPawn.Position = FVector::ZeroVector;
		SavedPawn.Rotation = FRotator::ZeroRotator;
		SavedController.Rotation = FRotator::ZeroRotator;
	}

	FORCEINLINE void ClearActorList()
	{
		ActorList.Empty();
		ActorMap.Empty();
	}

	FORCEINLINE void ClearSavedLevelActors()
	{
		SavedActors.Empty();
		SavedScripts.Empty();
	}

	FORCEINLINE void ClearStreamingData()
	{
		MultiLevelStreamData = FMultiLevelStreamingData();
	}

	FORCEINLINE void ClearWorldPartitionActors()
	{
		WorldPartitionActors.Empty();
	}

	FORCEINLINE void ClearDestroyedActors()
	{
		DestroyedActors.Empty();
	}

	FORCEINLINE void ClearPrunedLevelActors()
	{
		SavedActorsPruned.Empty();
	}

	FORCEINLINE void ClearUserData() override
	{
		Super::ClearUserData();

		//When setting/deleting a Save User, we need to clear this
		ClearMultiLevelSave();
		ClearWorldPartition();
	}

/** Actor Helpers  */

public:

	FORCEINLINE static FName LevelScriptSaveName(const AActor* Actor)
	{
		//Compare by level name, since the engine creates multiple script actors.
		return FName(FActorHelpers::GetActorLevelName(Actor));
	}

	FORCEINLINE static EActorType GetActorType(const AActor* Actor)
	{
		return FActorHelpers::GetActorType(Actor);
	}

	FORCEINLINE static FString GetFullActorName(const AActor* Actor)
	{
		return FActorHelpers::GetFullActorName(Actor);
	}

	FORCEINLINE static bool IsLoaded(const AActor* Actor)
	{
		return FActorHelpers::IsLoaded(Actor);
	}

	FORCEINLINE static bool IsSkipSave(const AActor* Actor)
	{
		return FActorHelpers::IsSkipSave(Actor);
	}

	FORCEINLINE static bool IsSkipTransform(const AActor* Actor)
	{
		return FActorHelpers::IsSkipTransform(Actor);
	}

	FORCEINLINE static bool IsLevelScript(const EActorType Type)
	{
		return FActorHelpers::IsLevelScript(Type);
	}

/** Other Helper Functions  */

public:

	FORCEINLINE static bool AutoDestroyActors()
	{
		return UEMSPluginSettings::Get()->bAutoDestroyActors;
	}

	FORCEINLINE static bool AdvancedSpawnCheck()
	{
		return UEMSPluginSettings::Get()->bAdvancedSpawnCheck;
	}

	FORCEINLINE bool HasSaveInterface(const AActor* Actor) const
	{
		return Actor->GetClass()->ImplementsInterface(UEMSActorSaveInterface::StaticClass());
	}

	FORCEINLINE bool HasComponentSaveInterface(const UActorComponent* Comp) const
	{
		return Comp && Comp->IsRegistered() && Comp->GetClass()->ImplementsInterface(UEMSCompSaveInterface::StaticClass());
	}

	FORCEINLINE bool IsValidActor(const AActor* Actor) const
	{
		return IsValid(Actor) && HasSaveInterface(Actor);
	}

	FORCEINLINE bool IsValidForSaving(const AActor* Actor) const
	{
		return IsValidActor(Actor) && !IsSkipSave(Actor);
	}

	FORCEINLINE bool IsValidForLoading(const AActor* Actor) const
	{
		return IsValidActor(Actor) && !IsLoaded(Actor);
	}

	FORCEINLINE bool AlwaysAutoLoadWorldPartition() const
	{
		return bInitWorldPartitionLoad && bLoadFromMemory;
	}

	FORCEINLINE TArray<FActorSaveData> GetSavedActors() const
	{
		return SavedActors;
	}

	FORCEINLINE TArray<FActorSaveData> GetPrunedLevelActors() const
	{
		return SavedActorsPruned;
	}

	FORCEINLINE FMultiLevelStreamingData GetMultiLevelStreamData() const
	{
		return MultiLevelStreamData;
	}

	FORCEINLINE bool HasLevelData() const
	{
		return !EMS::ArrayEmpty(SavedActors) 
			|| !EMS::ArrayEmpty(SavedScripts) 
			|| !EMS::ArrayEmpty(SavedGameMode.Data)
			|| !EMS::ArrayEmpty(SavedGameState.Data)
			|| MultiLevelStreamData.HasData();
	}

/** World Partition Helpers  */

public:

	FORCEINLINE bool AutoSaveLoadWorldPartition(UWorld* InWorld = nullptr) const
	{
		if (!InWorld)
		{
			InWorld = GetWorld();
		}

		return FStreamHelpers::AutoSaveLoadWorldPartition(InWorld);
	}

	FORCEINLINE bool CanProcessWorldPartition() const
	{
		return FStreamHelpers::CanProcessWorldPartition(GetWorld());
	}

	FORCEINLINE bool IsLevelStreaming()
	{
		return FStreamHelpers::IsLevelStillStreaming(GetWorld());
	}

	FORCEINLINE static bool SkipInitialWorldPartitionLoad()
	{
		return UEMSPluginSettings::Get()->WorldPartitionInit == EWorldPartitionInit::Skip;
	}
};
