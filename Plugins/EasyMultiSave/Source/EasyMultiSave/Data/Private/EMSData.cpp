//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#include "EMSData.h"
#include "EMSPluginSettings.h"
#include "EMSAsyncLoadGame.h"
#include "EMSAsyncSaveGame.h"
#include "EMSAsyncStream.h"
#include "Misc/Paths.h"
#include "Engine/Level.h"
#include "Engine/LevelScriptActor.h"
#include "Engine/LevelStreaming.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/UObjectIterator.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "ImageUtils.h"
#include "WorldPartition/WorldPartitionSubsystem.h"

DEFINE_LOG_CATEGORY(LogEasyMultiSave);

/**
FSaveVersion
**/

FString FSaveVersion::GetGameVersion()
{
	const FString CustomVersion = FString::FromInt(UEMSPluginSettings::Get()->SaveGameVersion);
	return EMS::VerGame + CustomVersion;
}

FSaveVersionInfo FSaveVersion::MakeSaveFileVersion()
{
	const FString EmsVersion = EMS::VerPlugin + VERSION_STRINGIFY(EMS_VERSION_NUMBER);
	const FString GameVersion = GetGameVersion();
	const FSaveVersionInfo Info = FSaveVersionInfo(EmsVersion, GameVersion);

	return Info;
}

bool FSaveVersion::IsSaveGameVersionEqual(const FSaveVersionInfo& SaveVersion)
{
	const FString GameVersion = GetGameVersion();
	return EMS::EqualString(SaveVersion.Game, GameVersion);
}

FPackageFileVersion FSaveVersion::GetStaticOldPackageVersion()
{
	//Check hardcoded package file versions. Print with GPackageFileUEVersion.ToValue()

	uint32 StaticPackageVersion = 1009;

	if (UEMSPluginSettings::Get()->MigratedSaveEngineVersion == EOldPackageEngine::EN_UE40)
	{
		StaticPackageVersion = 555;
	}
	else if (UEMSPluginSettings::Get()->MigratedSaveEngineVersion == EOldPackageEngine::EN_UE54)
	{
		StaticPackageVersion = 1012;
	}
	
	//1013 - UE 5.5+ - Not required as this will definitely have a written version.

	return FPackageFileVersion(StaticPackageVersion, EUnrealEngineObjectUE5Version(StaticPackageVersion));
}

 bool FSaveVersion::RequiresPerObjectPackageTag(const UObject* Object)
{
	if (!UEMSPluginSettings::Get()->bMigratedSaveActorVersionCheck)
	{
		return false;
	}

	if (FSettingHelpers::IsStackBasedMultiLevelSave() || FSettingHelpers::IsStreamMultiLevelSave())
	{
		if (const AActor* Actor = Cast<AActor>(Object))
		{
			const EActorType Type = FActorHelpers::GetActorType(Actor);
			if (FActorHelpers::IsLevelActor(Type, true))
			{
				return true;
			}
		}
		else
		{
			//This is for components. 
			return true;
		}
	}

	return false;
}

 void FSaveVersion::WriteObjectPackageTag(TArray<uint8>& Data)
 {
	 const uint8* DataTag = EMS::UE_OBJECT_PACKAGE_TAG;
	 Data.Append(DataTag, EMS_PKG_TAG_SIZE);
 }

 bool FSaveVersion::CheckObjectPackageTag(const TArray<uint8>& Data)
 {
	 const uint8* DataTag = EMS::UE_OBJECT_PACKAGE_TAG;
	 const uint8 Len = EMS_PKG_TAG_SIZE;

	 if (Data.Num() < Len)
	 {
		 return false;
	 }

	 //Compare the tag at the end of the array
	 for (int32 i = 0; i < Len; ++i)
	 {
		 if (Data[Data.Num() - Len + i] != DataTag[i])
		 {
			 return false;
		 }
	 }

	 return true;
 }

int32 FSaveVersion::UpdateArchiveVersion(FArchive& Ar, const int32 LatestVersion)
{
	//Versioning archives allows for easy modification of FLevelArchives, without the requirement to enumerate every entry 
	 
	int32 Version = LatestVersion;

	if (Ar.IsSaving())
	{
		Ar << Version;
	}
	else if (Ar.IsLoading())
	{
		//Try to read the version number
		int64 OriginalPosition = Ar.Tell();
		Ar << Version;

		//Handle empty version for legacy data
		if (Version <= 0)
		{
			Ar.Seek(OriginalPosition);
		}

		//Handle unexpected version
		if (Version > LatestVersion)
		{
			UE_LOG(LogEasyMultiSave, Warning, TEXT("Detected archive with a version higher than latest: %d (latest: %d)"), Version, LatestVersion);
		}
	}

	return Version;
}

EFileValidity FSaveVersion::IsSaveFileValid(const FString& InSavePath, const bool bLog)
{
	//For now, this only works on Desktop
#if !EMS_PLATFORM_DESKTOP
	return EFileValidity::FILE_VALID;
#else

	// File does not exist, so skip the check
	const FString FullSavePath = FString::Printf(EMS::NativeWindowsSavePath, *FPaths::ProjectSavedDir(), *InSavePath);
	if (!FPaths::FileExists(FullSavePath))
	{
		if (bLog) UE_LOG(LogEasyMultiSave, Log, TEXT("Save file missing, passed as valid since there is nothing to check - %s"), *FullSavePath);
		return EFileValidity::FILE_MISSING;
	}

	FArchive* FileReader = IFileManager::Get().CreateFileReader(*FullSavePath);
	if (!FileReader || FileReader->TotalSize() == 0)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Save file is empty or unreadable - %s"), *FullSavePath);
		return EFileValidity::FILE_INVALID;
	}

	const int64 FileSize = FileReader->TotalSize();

	//Check First 16 Bytes (NULL Check)
	uint8 FirstBytes[16] = { 0 };
	FileReader->Serialize(FirstBytes, sizeof(FirstBytes));

	bool bAllNull = true;
	FString HexDump;
	for (int32 i = 0; i < 16; i++)
	{
		HexDump += FString::Printf(TEXT("%02X "), FirstBytes[i]);
		if (FirstBytes[i] != 0) { bAllNull = false; }
	}

	if (bAllNull)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("File is filled with NULLs - corrupted save."));
		delete FileReader;
		return EFileValidity::FILE_INVALID;
	}

	//Check Last 16 Bytes (Version Number)
	int32 VersionCheckSize = 16;
	uint8 LastBytes[16] = { 0 };

	FileReader->Seek(FileSize - VersionCheckSize);
	FileReader->Serialize(LastBytes, VersionCheckSize);

	FString EndHexDump;
	for (int32 i = 0; i < VersionCheckSize; i++)
	{
		EndHexDump += FString::Printf(TEXT("%02X "), LastBytes[i]);
	}

	//Ensure File Reads Without Errors
	if (FileReader->IsError() || FileReader->Tell() > FileSize)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("File read error detected - corrupted save."));
		delete FileReader;
		return EFileValidity::FILE_INVALID;
	}

	//Close Archive and Return True
	FileReader->Close();
	delete FileReader;

	if (bLog) UE_LOG(LogEasyMultiSave, Log, TEXT("%s file successfully passed integrity check."), *FullSavePath);
	return EFileValidity::FILE_VALID;

#endif

}

/**
FSaveHelpers
**/

TArray<FString> FSaveHelpers::GetDefaultSaveFiles(const FString& SaveGameName)
{
	TArray<FString> AllFiles;

	using namespace EMS;
	{
		const FString PlayerFile = SaveGameName + Underscore + PlayerSuffix;
		const FString LevelFile = SaveGameName + Underscore + ActorSuffix;
		const FString SlotFile = SaveGameName + Underscore + SlotSuffix;
		const FString ThumbFile = SaveGameName + Underscore + ThumbSuffix;

		AllFiles.Add(PlayerFile);
		AllFiles.Add(LevelFile);
		AllFiles.Add(SlotFile);
		AllFiles.Add(ThumbFile);
	}

	return AllFiles;
}

TArray<uint8> FSaveHelpers::BytesFromString(const FString& String)
{
	const uint32 Size = String.Len();

	TArray<uint8> Bytes;
	Bytes.SetNumUninitialized(Size);
	StringToBytes(String, Bytes.GetData(), Size);

	return Bytes;
}

FString FSaveHelpers::StringFromBytes(const TArray<uint8>& Bytes)
{
	return BytesToString(Bytes.GetData(), Bytes.Num());
}

bool FSaveHelpers::CompareIdentifiers(const TArray<uint8>& ArrayId, const FString& StringId)
{
	if (StringId.Len() != ArrayId.Num())
	{
		return false;
	}

	return ArrayId == BytesFromString(StringId);
}

void FSaveHelpers::PruneSavedActors(const TMap<FName, const TWeakObjectPtr<AActor>>& InActorMap, TArray<FActorSaveData>& OutSaved)
{
	//Copy array, as it may be accessed from another thread
	TArray<FActorSaveData> PruneArray = OutSaved;

	//Reverse iterate through the array and remove unloaded placed Actors
	for (int32 i = PruneArray.Num() - 1; i >= 0; --i)
	{
		const FActorSaveData ActorArray = PruneArray[i];
		const FName ActorName = FActorHelpers::GetActorDataName(ActorArray);
		const TWeakObjectPtr<AActor> ActorPtr = InActorMap.FindRef(ActorName);

		const bool bValidPtr = ActorPtr.IsValid(false, true);
		const bool bWasLoaded = bValidPtr && ActorPtr.Get() && FActorHelpers::IsLoaded(ActorPtr.Get());

		if (!bValidPtr || bWasLoaded)
		{
			const EActorType Type = EActorType(ActorArray.Type);
			if (FActorHelpers::IsStreamRelevant(Type))
			{
				#if EMS_ENGINE_MIN_UE55
					OutSaved.RemoveAtSwap(i, 1, EAllowShrinking::No);
				#else
					OutSaved.RemoveAtSwap(i, 1, false);
				#endif
			}
		}
	}
}

bool FSaveHelpers::HasSaveArchiveError(const FBufferArchive& CheckArchive, ESaveErrorType ErrorType)
{
	FString ErrorString = FString();
	if (ErrorType == ESaveErrorType::ER_Player)
	{
		ErrorString = "Player";
	}
	else if (ErrorType == ESaveErrorType::ER_Level)
	{
		ErrorString = "Level";
	}
	else if (ErrorType == ESaveErrorType::ER_Object)
	{
		ErrorString = "Object(Slot Info or Custom Save)";
	}

	if (CheckArchive.IsCriticalError())
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("%s data contains critical errors and was not saved."), *ErrorString);
		return true;
	}

	if (CheckArchive.IsError())
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("%s data contains errors and was not saved."), *ErrorString);
		return true;
	}

	return false;
}

void FSaveHelpers::ExtractPlayerNames(const UWorld* InWorld, TArray<FString>& OutPlayerNames)
{
	if (!InWorld)
	{
		return;
	}

	if (const AGameStateBase* GameState = InWorld->GetGameState())
	{
		const TArray<APlayerState*> Players = GameState->PlayerArray;
		if (!EMS::ArrayEmpty(Players))
		{
			for (const APlayerState* PlayerState : Players)
			{
				OutPlayerNames.Add(PlayerState->GetPlayerName());
			}
		}
	}
}

/**
FActorHelpers
**/

FString FActorHelpers::GetActorLevelName(const AActor* Actor)
{
	if (!Actor)
	{
		return FString();
	}

	const ULevel* ActorLevel = Actor->GetLevel();
	if (!ActorLevel || !ActorLevel->GetOuter())
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Failed to return level for Actor: %s"), *Actor->GetName());
		return Actor->GetName();
	}

	const FString ActorLevelName = ActorLevel->GetOuter()->GetName();

	//Check if dynamic level instances are supported, this simply enumerates the parent level name.
	if (FSettingHelpers::IsDynamicLevelStreaming())
	{
		//@TODO when unloading and reloading, it will use new names. Would require manual Guid or a hash based on path, transform etc. But that is a hack.
		if (const ULevelStreamingDynamic* RuntimeLevelInstance = GetRuntimeLevelInstance(Actor))
		{	
			const FString InstanceName = RuntimeLevelInstance->GetName();
			const FString FinalInstanceName = InstanceName.Replace(EMS::RuntimeLevelInstance, TEXT(""));
			return ActorLevelName + FinalInstanceName;
		}
	}

	return ActorLevelName;
}

ULevelStreamingDynamic* FActorHelpers::GetRuntimeLevelInstance(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	const UWorld* World = Actor->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	//Return the associated runtime level instance
	if (FStreamHelpers::HasStreamingLevels(World))
	{
		for (ULevelStreaming* LevelStreaming : World->GetStreamingLevels())
		{
			ULevelStreamingDynamic* StreamingDynamic = Cast<ULevelStreamingDynamic>(LevelStreaming);
			if (StreamingDynamic && StreamingDynamic->GetLoadedLevel() == Actor->GetLevel())
			{
				return StreamingDynamic;
			}
		}
	}

	return nullptr;
}

FString FActorHelpers::GetFullActorName(const AActor* Actor)
{
	const FString ActorName = Actor->GetName();

	//World Partition has own unique Actor Ids
	if (FStreamHelpers::AutoSaveLoadWorldPartition())
	{
		return ActorName;
	}

	//This is only valid for placed Actors. Runtime Actors are always in the persistent.
	//Can't use GetActorType here, since it would crash Multi-Thread loading.
	if (IsPlacedActor(Actor))
	{
		const FString LevelString = FActorHelpers::GetActorLevelName(Actor);
		const bool bAlreadyHas = ActorName.Contains(LevelString);

		if (bAlreadyHas)
		{
			return ActorName;
		}
		else
		{
			return LevelString + EMS::Underscore + ActorName;
		}
	}

	return ActorName;
}

FString FActorHelpers::GetComponentName(const AActor* Actor, const UActorComponent* Comp)
{
    const FString CompName = Comp->GetName();

    //Edge case where a component owned by ActorB is saved from ActorA
    const AActor* CompOwner = Comp->GetOwner();
    if (CompOwner && CompOwner != Actor)
    {
        if (CompOwner->IsChildActor())
        {
            return CompOwner->GetParentComponent()->GetName() + CompName;
        }

        return GetFullActorName(CompOwner) + CompName;
    }

    return CompName;
}

FName FActorHelpers::GetWorldLevelName(const UWorld* InWorld)
{
	if (!InWorld)
	{
		return NAME_None;
	}

	//Get full path without PIE prefixes

	FString LevelName = InWorld->GetOuter()->GetName();
	const FString Prefix = InWorld->StreamingLevelsPrefix;

	const int Index = LevelName.Find(Prefix);
	const int Count = Prefix.Len();

	LevelName.RemoveAt(Index, Count);

	return FName(*LevelName);
}

FName FActorHelpers::GetActorDataName(const FActorSaveData& ActorData)
{
	const FString ActorStr = FSaveHelpers::StringFromBytes(ActorData.Name);
	const FName ActorName(*ActorStr);
	return ActorName;
}

bool FActorHelpers::IsMovable(const USceneComponent* SceneComp)
{
	if (SceneComp != nullptr)
	{
		return SceneComp->Mobility == EComponentMobility::Movable;
	}

	return false;
}

bool FActorHelpers::HasValidTransform(const FTransform& CheckTransform)
{
	return CheckTransform.IsValid() && CheckTransform.GetLocation() != FVector::ZeroVector;
}

bool FActorHelpers::CanProcessActorTransform(const AActor* Actor)
{
	return IsMovable(Actor->GetRootComponent()) && !IsSkipTransform(Actor) && Actor->GetAttachParentActor() == nullptr;
}

bool FActorHelpers::IsPlacedActor(const AActor* Actor)
{
	return Actor->IsNetStartupActor() || Actor->HasAnyFlags(RF_WasLoaded);
}

 bool FActorHelpers::IsPersistentActor(const AActor* Actor)
{
	return Actor->ActorHasTag(EMS::PersistentTag);
}

bool FActorHelpers::IsSkipTransform(const AActor* Actor)
{
	return Actor->ActorHasTag(EMS::SkipTransformTag);
}

bool FActorHelpers::IsLoaded(const AActor* Actor)
{
	return Actor->ActorHasTag(EMS::HasLoadedTag);
}

bool FActorHelpers::IsSkipSave(const AActor* Actor)
{
	return Actor->ActorHasTag(EMS::SkipSaveTag);
}

bool FActorHelpers::IsLevelActor(const EActorType Type, const bool bIncludeScripts)
{
	if (bIncludeScripts && Type == EActorType::AT_LevelScript)
	{
		return true;
	}

	return Type == EActorType::AT_Placed || Type == EActorType::AT_Runtime || Type == EActorType::AT_Persistent || Type == EActorType::AT_Destroyed;
}

bool FActorHelpers::IsStreamRelevant(const EActorType Type)
{
	return Type == EActorType::AT_Placed || Type == EActorType::AT_Destroyed;
}

bool FActorHelpers::IsRuntime(const EActorType Type)
{
	return Type == EActorType::AT_Runtime || Type == EActorType::AT_Persistent;
}

bool FActorHelpers::IsPersistent(const EActorType Type)
{
	return Type == EActorType::AT_Persistent;
}

bool FActorHelpers::IsLevelScript(const EActorType Type)
{
	return Type == EActorType::AT_LevelScript;
}

FString FActorHelpers::GetRawObjectID(const FRawObjectSaveData& Data)
{
	return Data.Id + EMS::RawObjectTag;
}

bool FActorHelpers::CompareDistance(const FVector& VecA, const FVector& VecB, const APlayerController* PC)
{
	if (PC && PC->PlayerCameraManager)
	{
		const FVector CameraLoc = PC->PlayerCameraManager->GetCameraLocation();
		const float DistA = FVector::Dist(VecA, CameraLoc);
		const float DistB = FVector::Dist(VecB, CameraLoc);
		return DistA < DistB;
	}

	return false;
}

void FActorHelpers::SetPlayerNotLoaded(APlayerController* PC)
{
	if (IsValid(PC))
	{
		PC->Tags.Remove(EMS::HasLoadedTag);

		if (APawn* Pawn = PC->GetPawn())
		{
			Pawn->Tags.Remove(EMS::HasLoadedTag);
		}

		if (APlayerState* PlayerState = PC->PlayerState)
		{
			PlayerState->Tags.Remove(EMS::HasLoadedTag);
		}
	}
}

EActorType FActorHelpers::GetActorType(const AActor* Actor)
{
	//Runtime spawned
	if (!IsValid(Actor))
	{
		return EActorType::AT_Runtime;
	}

	//Check if the actor is a Pawn and is controlled by a player
	if (const APawn* Pawn = Cast<APawn>(Actor))
	{
		if (Pawn->IsPlayerControlled())
		{
			return EActorType::AT_PlayerPawn;
		}
	}

	if (Cast<APlayerController>(Actor) || Cast<APlayerState>(Actor))
	{
		return EActorType::AT_PlayerActor;
	}

	if (Cast<ALevelScriptActor>(Actor))
	{
		return EActorType::AT_LevelScript;
	}

	if (Cast<AGameModeBase>(Actor) || Cast<AGameStateBase>(Actor))
	{
		return EActorType::AT_GameObject;
	}

	if (IsPersistentActor(Actor))
	{
		return EActorType::AT_Persistent;
	}

	if (IsPlacedActor(Actor))
	{
		return EActorType::AT_Placed;
	}

	return EActorType::AT_Runtime;
}

/**
FSpawnHelpers
**/

UClass* FSpawnHelpers::StaticLoadSpawnClass(const FString& Class)
{
	//Resolve directly
	UClass* ResolvedClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *Class, nullptr, LOAD_None, nullptr));
	if (ResolvedClass)
	{
		return ResolvedClass;
	}

	//Check for Redirects
	auto Redirectors = UEMSPluginSettings::Get()->RuntimeClasses;
	if (!Redirectors.IsEmpty())
	{
		const FSoftClassPath* RedirectedClass = Redirectors.Find(Class);
		if (RedirectedClass)
		{
			return RedirectedClass->TryLoadClass<AActor>();
		}
	}

	return nullptr;
}

UClass* FSpawnHelpers::ResolveSpawnClass(const FString& InClass)
{
	if (InClass.IsEmpty())
	{
		return nullptr;
	}

	UClass* SpawnClass = FindObject<UClass>(nullptr, *InClass);
	if (!SpawnClass)
	{
		SpawnClass = FSpawnHelpers::StaticLoadSpawnClass(InClass);
	}

	return SpawnClass;
}

FActorSpawnParameters FSpawnHelpers::GetSpawnParams(const TArray<uint8>& NameData)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Name = FName(*FSaveHelpers::StringFromBytes(NameData));
	SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
	return SpawnParams;
}

AActor* FSpawnHelpers::CheckForExistingActor(const UWorld* InWorld, const FActorSaveData& ActorArray)
{
	if (InWorld && InWorld->PersistentLevel)
	{
		const FName LoadedActorName(*FSaveHelpers::StringFromBytes(ActorArray.Name));
		AActor* NewLevelActor = Cast<AActor>(StaticFindObjectFast(nullptr, InWorld->PersistentLevel, LoadedActorName));
		if (NewLevelActor)
		{
			return NewLevelActor;
		}
	}

	return nullptr;
}

/**
FMultiLevelStreamingData
**/

template <typename TSaveData, typename TSaveDataArray>
void FMultiLevelStreamingData::ReplaceOrAddToArray(const TSaveData& Data, TSaveDataArray& OuputArray)
{
	//This will replace an existing element or add a new one. 
	const uint32 Index =  OuputArray.IndexOfByKey(Data);
	if (Index != INDEX_NONE)
	{
		OuputArray[Index] = Data;
	}
	else
	{
		OuputArray.Add(Data);
	}
}

void FMultiLevelStreamingData::CopyActors(const TArray<FActorSaveData>& InData)
{
	for (const FActorSaveData& ActorData : InData)
	{
		//We only add placed actors. All Actor types are stored in the SavedActors array.
		if (FActorHelpers::IsStreamRelevant(EActorType(ActorData.Type)))
		{
			const FName ActorKey(FSaveHelpers::StringFromBytes(ActorData.Name));
			ActorMap.Add(ActorKey, ActorData);
			ReplaceOrAddToArray(ActorData, ActorArray);
		}
	}
}

void FMultiLevelStreamingData::CopyTo(const FLevelArchive& A)
{
	CopyActors(A.SavedActors);

	for (const FLevelScriptSaveData& ScriptData : A.SavedScripts)
	{
		ReplaceOrAddToArray(ScriptData, ScriptArray);
	}
}

void FMultiLevelStreamingData::CopyFrom(FLevelArchive& A)
{
	const uint32 NumActors = A.SavedActors.Num() + ActorArray.Num();
	A.SavedActors.Reserve(NumActors);

	for (const FActorSaveData& ActorData : ActorArray)
	{
		ReplaceOrAddToArray(ActorData, A.SavedActors);
	}

	const uint32 NumScripts = A.SavedScripts.Num() + ScriptArray.Num();
	A.SavedScripts.Reserve(NumScripts);

	for (const FLevelScriptSaveData& ScriptData : ScriptArray)
	{
		ReplaceOrAddToArray(ScriptData, A.SavedScripts);
	}
}

/**
FStructHelpers
**/

void FStructHelpers::SerializeStruct(UObject* Object)
{
	//Non-array struct vars.
	for (TFieldIterator<FStructProperty> ObjectStruct(Object->GetClass()); ObjectStruct; ++ObjectStruct)
	{
		if (ObjectStruct && ObjectStruct->GetPropertyFlags() & CPF_SaveGame)
		{
			SerializeScriptStruct(ObjectStruct->Struct);
		}
	}

	//Struct-Arrays are cast as Arrays, not structs, so we work around it.
	for (TFieldIterator<FArrayProperty> ArrayProp(Object->GetClass()); ArrayProp; ++ArrayProp)
	{
		if (ArrayProp && ArrayProp->GetPropertyFlags() & CPF_SaveGame)
		{
			SerializeArrayStruct(*ArrayProp);
		}
	}

	//Map Properties
	for (TFieldIterator<FMapProperty> MapProp(Object->GetClass()); MapProp; ++MapProp)
	{
		if (MapProp && MapProp->GetPropertyFlags() & CPF_SaveGame)
		{
			SerializeMap(*MapProp);
		}
	}
}

void FStructHelpers::SerializeMap(FMapProperty* MapProp)
{
	FProperty* ValueProp = MapProp->ValueProp;
	if (ValueProp)
	{
		ValueProp->SetPropertyFlags(CPF_SaveGame);

		FStructProperty* ValueStructProp = CastField<FStructProperty>(ValueProp);
		if (ValueStructProp)
		{
			SerializeScriptStruct(ValueStructProp->Struct);
		}
	}
}

void FStructHelpers::SerializeArrayStruct(FArrayProperty* ArrayProp)
{
	FProperty* InnerProperty = ArrayProp->Inner;
	if (InnerProperty)
	{
		//Here we finally get to the structproperty, wich hides in the Array->Inner
		FStructProperty* ArrayStructProp = CastField<FStructProperty>(InnerProperty);
		if (ArrayStructProp)
		{
			SerializeScriptStruct(ArrayStructProp->Struct);
		}
	}
}

void FStructHelpers::SerializeScriptStruct(UStruct* ScriptStruct)
{
	if (ScriptStruct)
	{
		for (TFieldIterator<FProperty> Prop(ScriptStruct); Prop; ++Prop)
		{
			if (Prop)
			{
				Prop->SetPropertyFlags(CPF_SaveGame);

				//Recursive Array
				FArrayProperty* ArrayProp = CastField<FArrayProperty>(*Prop);
				if (ArrayProp)
				{
					SerializeArrayStruct(ArrayProp);
				}

				//Recursive Struct
				FStructProperty* StructProp = CastField<FStructProperty>(*Prop);
				if (StructProp)
				{
					SerializeScriptStruct(StructProp->Struct);
				}

				//Recursive Map
				FMapProperty* MapProp = CastField<FMapProperty>(*Prop);
				if (MapProp)
				{
					SerializeMap(MapProp);
				}
			}
		}
	}
}

/**
FSettingHelpers
**/

bool FSettingHelpers::IsNormalMultiLevelSave()
{
	return UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Normal;
}

bool FSettingHelpers::IsStreamMultiLevelSave()
{
	return UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Stream;
}

bool FSettingHelpers::IsFullMultiLevelSave()
{
	return UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Full;
}

bool FSettingHelpers::IsStackBasedMultiLevelSave()
{
	return IsFullMultiLevelSave() || IsNormalMultiLevelSave();
}

bool FSettingHelpers::IsContainingStreamMultiLevelSave()
{
	return IsFullMultiLevelSave() || IsStreamMultiLevelSave();
}

bool FSettingHelpers::IsDynamicLevelStreaming()
{
	return IsContainingStreamMultiLevelSave() && UEMSPluginSettings::Get()->bDynamicLevelStreaming;
}

bool FSettingHelpers::IsMemoryOnlySave()
{
	return UEMSPluginSettings::Get()->WorldPartitionSaving == EWorldPartitionMethod::MemoryOnly;
}

bool FSettingHelpers::IsConsoleFileSystem()
{
	return UEMSPluginSettings::Get()->FileSaveMethod == EFileSaveMethod::FM_Console;
}

bool FSettingHelpers::IsPersistentGameMode()
{
	return UEMSPluginSettings::Get()->bPersistentGameMode 
		&& UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Disabled;
}

bool FSettingHelpers::IsPersistentPlayer()
{
	return UEMSPluginSettings::Get()->bPersistentPlayer
		&& UEMSPluginSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Disabled;
}

bool FSettingHelpers::IsMultiThreadSaving()
{
	return UEMSPluginSettings::Get()->bMultiThreadSaving && FPlatformProcess::SupportsMultithreading();
}

bool FSettingHelpers::IsMultiThreadLoading()
{
	return UEMSPluginSettings::Get()->LoadMethod == ELoadMethod::LM_Thread && FPlatformProcess::SupportsMultithreading();
}

bool FSettingHelpers::IsDeferredLoading()
{
	return UEMSPluginSettings::Get()->LoadMethod == ELoadMethod::LM_Deferred;
}

uint32 FSettingHelpers::GetLoadBatchSize()
{
	return FMath::Max(1, UEMSPluginSettings::Get()->DeferredLoadStackSize);
}

/**
FStreamHelpers
**/

bool FStreamHelpers::AutoSaveLoadWorldPartition(const UWorld* InWorld)
{
	if (!IsValid(InWorld))
	{
		return false;
	}

	if (UEMSPluginSettings::Get()->WorldPartitionSaving == EWorldPartitionMethod::Disabled)
	{
		return false;
	}

	if (FSettingHelpers::IsContainingStreamMultiLevelSave() && InWorld->GetWorldPartition())
	{
		return true;
	}

	return false;
}

bool FStreamHelpers::HasStreamingLevels(const UWorld* InWorld)
{
	if (!IsValid(InWorld))
	{
		return false;
	}

	if (AutoSaveLoadWorldPartition(InWorld))
	{
		return true;
	}

	return !InWorld->GetStreamingLevels().IsEmpty();
}

bool FStreamHelpers::IsLevelStillStreaming(const UWorld* InWorld)
{
	if (!IsValid(InWorld))
	{
		return true;
	}

	if (!HasStreamingLevels(InWorld))
	{
		return false;
	}

	//Check to see if the subsystem has something to say
	if (AutoSaveLoadWorldPartition())
	{
		if (UWorldPartitionSubsystem* WorldPartitionSubsystem = InWorld->GetSubsystem<UWorldPartitionSubsystem>())
		{
			if (!WorldPartitionSubsystem->IsAllStreamingCompleted())
			{
				return true;
			}
		}
	}

	for (const ULevelStreaming* StreamingLevel : InWorld->GetStreamingLevels())
	{
		if (!StreamingLevel)
		{
			continue;
		}

		const ELevelStreamingState StreamingState = StreamingLevel->GetLevelStreamingState();

		//These states are not relevant at all, since the Actors are ignored anyway.
		if (StreamingState == ELevelStreamingState::FailedToLoad 
			|| StreamingState == ELevelStreamingState::Removed
			|| StreamingState == ELevelStreamingState::Unloaded
			|| StreamingState == ELevelStreamingState::LoadedNotVisible )
		{
			return false;
		}

		//All other states will block the async save/load operations.
		if (StreamingState != ELevelStreamingState::LoadedVisible)
		{
			return true;
		}
	}

	return false;
}

bool FStreamHelpers::IsWorldPartitionInit(const UWorld* InWorld)
{
	return InWorld->TimeSeconds < UEMSPluginSettings::Get()->WorldPartitionInitTime;
}

bool FStreamHelpers::CanProcessWorldPartition(const UWorld* InWorld)
{
	const bool bInit = FStreamHelpers::IsWorldPartitionInit(InWorld);
	const bool bIsStreaming = FStreamHelpers::IsLevelStillStreaming(InWorld);
	const bool bIsAsync = FAsyncSaveHelpers::IsAsyncSaveOrLoadTaskActive(InWorld, ESaveGameMode::MODE_All, EAsyncCheckType::CT_Both, false);
	return !bInit && !bIsStreaming && !bIsAsync;
}

/**
FSavePaths
**/

FString FSavePaths::GetBackupSavePath(const FString& FullSavePath)
{
	FString SavePath = FullSavePath;

	if (FSettingHelpers::IsConsoleFileSystem())
	{
		SavePath = FullSavePath + EMS::BackupTag;
	}
	else
	{
		const int32 SlashIndex = SavePath.Find(EMS::Slash);
		if (SlashIndex != INDEX_NONE)
		{
			const FString Directory = SavePath.Left(SlashIndex) + EMS::BackupTag;
			const FString FileName = SavePath.Mid(SlashIndex + 1);
			SavePath = Directory + EMS::Slash + FileName;
		}
	}

	return SavePath;
}

TArray<FString> FSavePaths::GetSortedSaveSlots(TArray<FSaveSlotInfo>& SaveSlots)
{
	SaveSlots.Sort([](const FSaveSlotInfo& A, const FSaveSlotInfo& B)
	{
		return A.TimeStamp > B.TimeStamp;
	});

	TArray<FString> SaveSlotNames;
	for (const FSaveSlotInfo& SlotInfo : SaveSlots)
	{
		//Skip backup tag
		if (SlotInfo.Name.Contains(EMS::BackupTag))
		{
			continue;
		}

		SaveSlotNames.Add(SlotInfo.Name);
	}

	return SaveSlotNames;
}

FString FSavePaths::ValidateSaveName(const FString& SaveGameName)
{
	FString CurrentSave = SaveGameName;
	CurrentSave = CurrentSave.Replace(TEXT(" "), *EMS::Underscore);
	CurrentSave = CurrentSave.Replace(TEXT("."), *EMS::Underscore);

	return FPaths::MakeValidFileName(*CurrentSave);
}

FString FSavePaths::GetThumbnailFormat()
{
	if (UEMSPluginSettings::Get()->ThumbnailFormat == EThumbnailImageFormat::Png)
	{
		return EMS::ImgFormatPNG;
	}

	return EMS::ImgFormatJPG;
}

FString FSavePaths::GetThumbnailFileExtension()
{
	if (FSettingHelpers::IsConsoleFileSystem())
	{
		return EMS::SaveType;
	}

	return TEXT(".") + GetThumbnailFormat();
}

void FSavePaths::CheckForReadOnly(const FString& FullSavePath)
{
	const FString NativePath = FString::Printf(EMS::NativeWindowsSavePath, *FPaths::ProjectSavedDir(), *FullSavePath);
	const bool bReadOnly = IFileManager::Get().IsReadOnly(*NativePath);
	if (bReadOnly)
	{
		FPlatformFileManager::Get().GetPlatformFile().SetReadOnly(*NativePath, false);
		UE_LOG(LogEasyMultiSave, Warning, TEXT("File access read only was set to false: %s"), *NativePath);
	}
}

TArray<FString> FSavePaths::GetConsoleSlotFiles(const TArray<FString>& SaveGameNames)
{
	const FString FullSlotSuffix = EMS::Underscore + EMS::SlotSuffix;

	//Filter out slots
	TArray<FString> SlotNames;
	for (const FString& ActualFileName : SaveGameNames)
	{
		const bool bIsActualSlot = ActualFileName.Contains(FullSlotSuffix);
		if (bIsActualSlot)
		{
			//Get actual name without suffix
			const int32 Index = ActualFileName.Find(FullSlotSuffix, ESearchCase::IgnoreCase, ESearchDir::FromEnd, INDEX_NONE);
			const int32 Count = FullSlotSuffix.Len();

			FString ReducedFileName = ActualFileName;
			ReducedFileName.RemoveAt(Index, Count);
			SlotNames.Add(ReducedFileName);
		}
	}

	return SlotNames;
}

/**
Async Node Helper Functions
**/

template<class T>
bool FAsyncSaveHelpers::CheckLoadIterator(const T& It, const ESaveGameMode Mode, const bool bLog, const FString& DebugString)
{
	if (It && It->IsActive() && (It->Mode == Mode || Mode == ESaveGameMode::MODE_All))
	{
		if (bLog)
		{
			UE_LOG(LogEasyMultiSave, Warning, TEXT("%s is active while trying to save or load."), *DebugString);
		}

		return true;
	}

	return false;
}

bool FAsyncSaveHelpers::IsAsyncSaveOrLoadTaskActive(const UWorld* InWorld, const ESaveGameMode Mode, const EAsyncCheckType CheckType, const bool bLog)
{
	//This will prevent the functions from being executed at all during pause.
	if (InWorld->IsPaused())
	{
		if (bLog)
		{
			UE_LOG(LogEasyMultiSave, Warning, TEXT("Async save or load called during pause. Operation was canceled."));
		}

		return true;
	}

	if (CheckType == EAsyncCheckType::CT_Both || CheckType == EAsyncCheckType::CT_Load)
	{
		for (TObjectIterator<UEMSAsyncLoadGame> It; It; ++It)
		{
			if (CheckLoadIterator(It, Mode, bLog, "Load Game Actors"))
			{
				return true;
			}
		}

		for (TObjectIterator<UEMSAsyncStream> It; It; ++It)
		{
			if (CheckLoadIterator(It, Mode, bLog, "Load Stream Level Actors"))
			{
				return true;
			}
		}
	}

	if (CheckType == EAsyncCheckType::CT_Both || CheckType == EAsyncCheckType::CT_Save)
	{
		for (TObjectIterator<UEMSAsyncSaveGame> It; It; ++It)
		{
			if (CheckLoadIterator(It, Mode, bLog, "Save Game Actors"))
			{
				return true;
			}
		}
	}

	return false;
}

bool FAsyncSaveHelpers::IsStreamAutoLoadActive(const ULevel* InLevel)
{
	for (TObjectIterator<UEMSAsyncStream> It; It; ++It)
	{
		if (It && It->IsActive() && It->StreamingLevel == InLevel)
		{
			return true;
		}
	}

	return false;
}

void FAsyncSaveHelpers::DestroyStreamAutoLoadTask(const ULevel* InLevel)
{
	for (TObjectIterator<UEMSAsyncStream> It; It; ++It)
	{
		if (It && It->IsActive() && It->StreamingLevel == InLevel)
		{
			It->ForceDestroy();
		}
	}
}

void FAsyncSaveHelpers::DestroyAsyncLoadLevelTask()
{
	for (TObjectIterator<UEMSAsyncLoadGame> It; It; ++It)
	{
		if (It && It->IsActive())
		{
			It->ForceDestroy();
		}
	}
}

ESaveGameMode FAsyncSaveHelpers::GetMode(const int32 Data)
{
	if (Data & ENUM_TO_FLAG(ESaveTypeFlags::SF_Player))
	{
		if (Data & ENUM_TO_FLAG(ESaveTypeFlags::SF_Level))
		{
			return ESaveGameMode::MODE_All;
		}
		else
		{
			return ESaveGameMode::MODE_Player;
		}
	}

	return ESaveGameMode::MODE_Level;
}

/**
Thumbnail Functions
**/

bool FSaveThumbnails::HasRenderTargetResource(UTextureRenderTarget2D* TextureRenderTarget)
{
	return TextureRenderTarget->GetResource() != nullptr;
}

bool FSaveThumbnails::CompressRenderTarget(UTextureRenderTarget2D* TexRT, FArchive& Ar)
{
	FImage Image;
	if (!FImageUtils::GetRenderTargetImage(TexRT, Image))
	{
		return false;
	}

	TArray64<uint8> CompressedData;
	if (!FImageUtils::CompressImage(CompressedData, *FSavePaths::GetThumbnailFormat(), Image, 90))
	{
		return false;
	}

	Ar.Serialize((void*)CompressedData.GetData(), CompressedData.GetAllocatedSize());

	return true;
}

bool FSaveThumbnails::ExportRenderTarget(UTextureRenderTarget2D* TexRT, const FString& FileName)
{
	FArchive* Ar = IFileManager::Get().CreateFileWriter(*FileName);
	if (Ar)
	{
		FBufferArchive Buffer;
		if (CompressRenderTarget(TexRT, Buffer))
		{
			Ar->Serialize(const_cast<uint8*>(Buffer.GetData()), Buffer.Num());
			delete Ar;

			return true;
		}
	}

	return false;
}