//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#include "EMSObjectBase.h"
#include "EMSCustomSaveGame.h"
#include "EMSInfoSaveGame.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "ImageUtils.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameSystem.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformFileManager.h"
#include "PlatformFeatures.h"

/**
Init
**/

UEMSObjectBase::UEMSObjectBase()
{
	bLoadFromMemory = false;
	LoadedPackageVersion = GPackageFileUEVersion;
	LastSlotSaveTime = 0.f;
}

void UEMSObjectBase::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogEasyMultiSave, Log, TEXT("Easy Multi Save Initialized"));

	const FString VersionNum = VERSION_STRINGIFY(EMS_VERSION_NUMBER);
	UE_LOG(LogEasyMultiSave, Log, TEXT("Easy Multi Save Version: %s"), *VersionNum);

	UE_LOG(LogEasyMultiSave, Log, TEXT("Current Save Game Slot: %s"), *GetCurrentSaveGameName());
}

UWorld* UEMSObjectBase::GetWorld() const
{
	return GetGameInstance()->GetWorld();
}

const bool UEMSObjectBase::HasValidWorld() const
{
	const TWeakObjectPtr<UWorld> TheWorld = MakeWeakObjectPtr(GetWorld());
	return TheWorld.IsValid();
}

/**
Save Object Creation
**/

template <class TSaveGame>
TSaveGame* UEMSObjectBase::CreateNewSaveObject(const FString& FullSavePath, const FSoftClassPath& InClassName)
{
	if (FullSavePath.IsEmpty())
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Save path is empty"));
		return nullptr;
	}

	//Try to load the class from the provided class name
	const FSoftClassPath LocalClassName = InClassName;
	TSubclassOf<TSaveGame> Class = LocalClassName.TryLoadClass<TSaveGame>();

	if (!Class)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Invalid Save Game Object Class: %s"), *LocalClassName.ToString());
		return nullptr;
	}

	//Load save game object 
	USaveGame* SaveGame = NewObject<USaveGame>(this, Class);
	if (!SaveGame)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Failed to load Save Game Object: %s"), *FullSavePath);
		return nullptr;
	}

	TSaveGame* SaveGameObject = Cast<TSaveGame>(SaveGame);
	if (!SaveGameObject)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Invalid Save Game Object: %s"), *FullSavePath);
		return nullptr;
	}

	//Check integrity and only unpack when loading(when its actually there), this is the safe way for the unified getter functions
	const EFileValidity ValidCheck = IsSaveFileValid(FullSavePath, false);
	if (ValidCheck == EFileValidity::FILE_VALID)
	{
		LoadBinaryArchive(EDataLoadType::DATA_Object, FullSavePath, SaveGameObject);
	}

	return SaveGameObject;
}

/**
Custom Save Objects
**/

UEMSCustomSaveGame* UEMSObjectBase::GetCustomSave(const TSubclassOf<UEMSCustomSaveGame>& SaveGameClass, const FString& InSlotName, const FString& InFileName)
{
	if (!SaveGameClass)
	{
		return nullptr;
	}

	const UEMSCustomSaveGame* CustomClass = Cast<UEMSCustomSaveGame>(SaveGameClass->GetDefaultObject());
	if (!CustomClass)
	{
		return nullptr;
	}

	//Allow for custom file names
	const FString CustomSaveName = [InFileName, CustomClass]() -> FString
	{
		if (InFileName.IsEmpty())
		{
			return CustomClass->SaveGameName.IsEmpty() ? CustomClass->GetName() : CustomClass->SaveGameName;
		}
		else
		{
			return InFileName;
		}
	}();

	const bool bUseSlot = CustomClass->bUseSaveSlot;
	const bool bCustomSlotName = bUseSlot && !InSlotName.IsEmpty();

	const FString ActualSlot = bCustomSlotName ? InSlotName : GetCurrentSaveGameName();
	const FString SlotName = bUseSlot ? ActualSlot : FString();
	const FString CachedRefName = CustomSaveName + SlotName;

	if (UEMSCustomSaveGame* CachedObject = CachedCustomSaves.FindRef(CachedRefName))
	{
		if (bUseSlot) CachedObject->SlotName = SlotName;
		return CachedObject;
	}

	const FString SaveFile = CustomSaveFile(CustomSaveName, SlotName);
	const FSoftClassPath SaveClass = CustomClass->GetClass();

	if (UEMSCustomSaveGame* NewObject = CreateNewSaveObject<UEMSCustomSaveGame>(SaveFile, SaveClass))
	{
		NewObject->SaveGameName = CustomSaveName;
		NewObject->SlotName = SlotName;
		CachedCustomSaves.Add(CachedRefName, NewObject);
		return NewObject;
	}

	return nullptr;
}

bool UEMSObjectBase::SaveCustom(UEMSCustomSaveGame* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		return false;
	}

	const bool bUseSlot = SaveGame->bUseSaveSlot;
	const FString SlotName = bUseSlot ? SaveGame->SlotName : FString();
	const FString CustomSaveName = SaveGame->SaveGameName;

	if (SaveObject(*CustomSaveFile(CustomSaveName, SlotName), SaveGame))
	{
		if (bUseSlot)
		{
			SaveSlotInfoObject(SlotName);
			UE_LOG(LogEasyMultiSave, Log, TEXT("Custom Save Game saved: %s for Slot: %s"), *CustomSaveName, *SlotName);
		}
		else
		{
			UE_LOG(LogEasyMultiSave, Log, TEXT("Custom Save Game saved: %s"), *CustomSaveName);
		}

		return true;
	}

	UE_LOG(LogEasyMultiSave, Warning, TEXT("Failed to save Custom Save Game: %s"), *CustomSaveName);

	return false;
}

bool UEMSObjectBase::SaveAllCustomObjects()
{
	bool bSuccess = false;

	for (auto& CustomObjectPair : CachedCustomSaves)
	{
		UEMSCustomSaveGame* SaveGame = CustomObjectPair.Value;
		if (!SaveGame)
		{
			continue;
		}

		//It can only auto-save objects in the current slot, but you can still manually save in other slots
		if (SaveGame->bUseSaveSlot)
		{
			if (SaveGame->SlotName != GetCurrentSaveGameName())
			{
				continue;
			}
		}

		bSuccess |= SaveCustom(SaveGame);
	}

	return bSuccess;
}

void UEMSObjectBase::DeleteCustomSave(UEMSCustomSaveGame* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		return;
	}

	const bool bUseSlot = SaveGame->bUseSaveSlot;
	const FString CustomSaveName = SaveGame->SaveGameName;
	const FString SlotName = bUseSlot ? SaveGame->SlotName : FString();
	const FString SaveFile = CustomSaveFile(CustomSaveName, SlotName);

	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (SaveSystem->DoesSaveGameExist(*SaveFile, PlayerIndex))
	{
		if (SaveSystem->DeleteGame(false, *SaveFile, PlayerIndex))
		{
			const FString CachedRefName = bUseSlot ? CustomSaveName + SlotName : CustomSaveName;
			CachedCustomSaves.Remove(CachedRefName);

			UE_LOG(LogEasyMultiSave, Log, TEXT("Custom Save Game Deleted: %s"), *CustomSaveName);
		}
	}
}

void UEMSObjectBase::ClearCachedCustomSaves()
{
	CachedCustomSaves.Empty();
}

void UEMSObjectBase::ClearCustomSavesDesktop(const FString& SaveGameName)
{
	//Only clears the cache
	TArray<FString> TempArray;
	ClearCustomSavesConsole(SaveGameName, false, TempArray);
}

void UEMSObjectBase::ClearCustomSavesConsole(const FString& SaveGameName, const bool bAddFiles, TArray<FString>& OutFiles)
{
	TMap<FString, TObjectPtr<UEMSCustomSaveGame>> TempCustomSaves = CachedCustomSaves;

	for (auto It = TempCustomSaves.CreateIterator(); It; ++It)
	{
		if (It->Key.Contains(SaveGameName))
		{
			const TObjectPtr<UEMSCustomSaveGame> CustomSaveGame = It->Value;
			if (CustomSaveGame)
			{
				//@TODO cannot delete if the slot is not cached, non-cached custom saves must be removed manually on console.
				//We cannot iterate the files, so we need another solution.
				if (bAddFiles)
				{
					//Used to delete custom saves for a slot with the console file system
					const FString CustomName = SaveGameName + EMS::Underscore + CustomSaveGame->SaveGameName;
					OutFiles.Add(CustomName);
				}

				CachedCustomSaves.Remove(It->Key);
			}
		}
	}
}

/**
Save Slots
**/

FString UEMSObjectBase::GetCurrentSaveGameName() const
{
	const FString DefaultName = UEMSPluginSettings::Get()->DefaultSaveGameName;

	if (CurrentSaveGameName.IsEmpty())
	{
		return DefaultName;
	}

	return CurrentSaveGameName;
}

void UEMSObjectBase::SetCurrentSaveGameName(const FString& SaveGameName)
{
	if (CurrentSaveGameName != SaveGameName)
	{
		//When switching slots, we want to always load from disk
		ClearLoadFromMemory();

		CurrentSaveGameName = FSavePaths::ValidateSaveName(SaveGameName);
		SaveConfig();

		UE_LOG(LogEasyMultiSave, Log, TEXT("New Current Save Game Slot is: %s"), *CurrentSaveGameName);
	}
}

TArray<FString> UEMSObjectBase::GetSortedSaveSlots() const
{
	TArray<FString> SaveGameNames;

	if (IsConsoleFileSystem())
	{
		//Files
		SaveGameNames = FSavePaths::GetConsoleSlotFiles(GetAllSaveGames());
	}
	else
	{
		//Folders
		IFileManager::Get().FindFiles(SaveGameNames, *FPaths::Combine(BaseSaveDir(), TEXT("*")), false, true);
	}

	//Return list sorted by time
	TArray<FSaveSlotInfo> SaveSlots = GetSlotInfos(SaveGameNames);
	return FSavePaths::GetSortedSaveSlots(SaveSlots);
}

TArray<FString> UEMSObjectBase::GetAllSaveGames() const
{
	TArray<FString> SaveGameNames;

	//Might not be available on all platforms
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	SaveSystem->GetSaveGameNames(SaveGameNames, PlayerIndex);

	return SaveGameNames;
}

TArray<FSaveSlotInfo> UEMSObjectBase::GetSlotInfos(const TArray<FString>& SaveGameNames) const
{
	//Fill with proper info
	TArray<FSaveSlotInfo> SaveSlots;
	for (const FString& SlotName : SaveGameNames)
	{
		FSaveSlotInfo SlotInfo;
		const FString SlotPath = SlotFilePath(SlotName);
		SlotInfo.TimeStamp = IFileManager::Get().GetTimeStamp(*SlotPath);
		SlotInfo.Name = SlotName;

		SaveSlots.Add(SlotInfo);
	}

	return SaveSlots;
}

UEMSInfoSaveGame* UEMSObjectBase::GetSlotInfoObject(const FString& SaveGameName)
{
	//Return the cached reference or create a new slot object

	if (UEMSInfoSaveGame* CachedSlot = CachedSaveSlots.FindRef(SaveGameName))
	{
		return CachedSlot;
	}

	if (UEMSInfoSaveGame* NewSlot = MakeSlotInfoObject(SaveGameName))
	{
		CachedSaveSlots.Add(SaveGameName, NewSlot);
		return NewSlot;
	}

	return nullptr;
}

UEMSInfoSaveGame* UEMSObjectBase::MakeSlotInfoObject(const FString& SaveGameName)
{
	const FString SaveSlotFile = SlotInfoSaveFile(SaveGameName);

	//Workaround for when Class can not be loaded from the DefaultEngine.ini in a cooked version
	FSoftClassPath SaveSlotClass = UEMSPluginSettings::Get()->SlotInfoSaveGameClass;
	if (SaveSlotClass.IsNull() || !SaveSlotClass.IsValid())
	{
		SaveSlotClass = EMS::DefaultSlotClass;
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Slot Info Class could not be read from settings, using default."));
	}

	if (UEMSInfoSaveGame* NewSlot = CreateNewSaveObject<UEMSInfoSaveGame>(SaveSlotFile, SaveSlotClass))
	{
		return NewSlot;
	}

	return nullptr;
}

void UEMSObjectBase::SaveSlotInfoObject(const FString& SaveGameName)
{
	if (SaveGameName.IsEmpty())
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Trying to save slot with an empty name"));
		return;
	}

	//Prevent redundant operations if slot exists and delay hasn't passed, using platform time
	if (FPlatformTime::Seconds() - LastSlotSaveTime < EMS::ShortDelay)
	{
		if (DoesSaveGameExist(SaveGameName))
		{
			return;
		}
	}

	if (!VerifyOrCreateDirectory(SaveGameName))
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Failed to verify or create directory for: %s"), *SaveGameName);
		return;
	}

	//Retrieve or create save object
	UEMSInfoSaveGame* SaveGame = CachedSaveSlots.FindRef(SaveGameName);
	if (!IsValid(SaveGame))
	{
		SaveGame = MakeSlotInfoObject(SaveGameName);
	}

	if (!SaveGame)
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Invalid Save Object for: %s"), *SaveGameName);
		return;
	}

	//Update save info
	SaveGame->SlotInfo.Name = SaveGameName;
	SaveGame->SlotInfo.TimeStamp = FDateTime::Now();
	SaveGame->SlotInfo.Level = GetLevelName();

	TArray<FString> PlayerNames;
	FSaveHelpers::ExtractPlayerNames(GetWorld(), PlayerNames);
	SaveGame->SlotInfo.Players = PlayerNames;

	//Update cache, otherwise it will never overwrite the data during a session
	CachedSaveSlots.Add(SaveGameName, SaveGame);

	//Save object
	if (SaveObject(*SlotInfoSaveFile(SaveGameName), SaveGame))
	{
		UE_LOG(LogEasyMultiSave, Log, TEXT("Slot Info saved: %s"), *SaveGameName);
	}
	else
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Slot Info could not be saved: %s"), *SaveGameName);
	}

	LastSlotSaveTime = FPlatformTime::Seconds();
}

bool UEMSObjectBase::DoesSaveGameExist(const FString& SaveGameName) const
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();

	const bool bHasSlotFile = SaveSystem->DoesSaveGameExist(*SlotInfoSaveFile(SaveGameName), PlayerIndex);
	return bHasSlotFile;
}

bool UEMSObjectBase::DoesFullSaveGameExist(const FString& SaveGameName) const
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();

	const bool bHasSlotFile = SaveSystem->DoesSaveGameExist(*SlotInfoSaveFile(SaveGameName), PlayerIndex);
	const bool bHasPlayerFile = SaveSystem->DoesSaveGameExist(*PlayerSaveFile(SaveGameName), PlayerIndex);
	const bool bHasLevelFile = SaveSystem->DoesSaveGameExist(*ActorSaveFile(SaveGameName), PlayerIndex);
	return bHasSlotFile && bHasPlayerFile && bHasLevelFile;
}

void UEMSObjectBase::DeleteAllSaveDataForSlot(const FString& SaveGameName)
{
	bool bSuccess = false;

	//Console uses files and not folders
	if (IsConsoleFileSystem())
	{
		//Hardcoded default files, since we cannot iterate through them
		TArray<FString> AllFiles = FSaveHelpers::GetDefaultSaveFiles(SaveGameName);

		//Parse the custom save objects and clear their cache
		ClearCustomSavesConsole(SaveGameName, true, AllFiles);

		//Use native delete 
		ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
		for (const FString& FileName : AllFiles)
		{
			if (*FileName)
			{
				SaveSystem->DeleteGame(false, *FileName, PlayerIndex);
				bSuccess = true;
			}
		}

		if (bSuccess)
		{
			UE_LOG(LogEasyMultiSave, Log, TEXT("Save Game Data removed for: %s"), *SaveGameName);
		}
	}
	else
	{
		const FString SaveFile = FPaths::Combine(BaseSaveDir(), SaveGameName);

		bSuccess = IFileManager::Get().DeleteDirectory(*SaveFile, true, true);
		if (bSuccess)
		{
			UE_LOG(LogEasyMultiSave, Log, TEXT("Save Game Data removed for: %s"), *SaveGameName);
		}

		//Delete the cached custom save objects
		ClearCustomSavesDesktop(SaveGameName);
	}

	//Remove Cached Slot
	CachedSaveSlots.Remove(SaveGameName);
}

void UEMSObjectBase::ClearCachedSlots()
{
	CachedSaveSlots.Empty();
}

/**
Save Users
**/

FString UEMSObjectBase::GetCurrentSaveUserName() const
{
	return CurrentSaveUserName;
}

bool UEMSObjectBase::HasSaveUserName() const
{
	return !GetCurrentSaveUserName().IsEmpty();
}

void UEMSObjectBase::SetCurrentSaveUserName(const FString& UserName)
{
	if (IsConsoleFileSystem())
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Save Users are not supported when using the console file system."));
		return;
	}

	if (CurrentSaveUserName != UserName)
	{
		ClearUserData();

		CurrentSaveUserName = UserName;
		SaveConfig();

		UE_LOG(LogEasyMultiSave, Log, TEXT("New Current Save User Name is: %s"), *UserName);
	}
}

void UEMSObjectBase::DeleteAllSaveDataForUser(const FString& UserName)
{
	ClearUserData();

	const FString UserSaveFile = SaveUserDir() + UserName;
	bool bSuccess = false;

	//Try removing folder	
	bSuccess = IFileManager::Get().DeleteDirectory(*UserSaveFile, true, true);
	if (bSuccess)
	{
		UE_LOG(LogEasyMultiSave, Log, TEXT("Save Game User Data removed for: %s"), *UserName);
	}
}

TArray<FString> UEMSObjectBase::GetAllSaveUsers() const
{
	TArray<FString> SaveUserNames;
	IFileManager::Get().FindFiles(SaveUserNames, *FPaths::Combine(SaveUserDir(), TEXT("*")), false, true);

	return SaveUserNames;
}

void UEMSObjectBase::ClearUserData()
{
	ClearCachedSlots();
	ClearCachedCustomSaves();
}

/**
Save and Load Archive Functions
**/

bool UEMSObjectBase::SaveObject(const FString& FullSavePath, UObject* SaveGameObject) const
{
	bool bSuccess = false;

	if (SaveGameObject)
	{
		TArray<uint8> Data;

		FMemoryWriter MemoryWriter(Data, true);
		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
		SaveGameObject->Serialize(Ar);

		FBufferArchive Archive;
		WritePackageInfo(Archive);
		Archive << Data;

		//Check archive errors directly
		if (!FSaveHelpers::HasSaveArchiveError(Archive, ESaveErrorType::ER_Object))
		{
			bSuccess = SaveBinaryArchive(Archive, *FullSavePath);
		}
	}

	return bSuccess;
}

bool UEMSObjectBase::SaveBinaryData(const TArray<uint8>& SavedData, const FString& FullSavePath) const
{
	//Auto backup 
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (UEMSPluginSettings::Get()->bAutoBackup && SaveSystem->DoesSaveGameExist(*FullSavePath, PlayerIndex))
	{
		PerformAutoBackup(FullSavePath);
	}

	//Save new data
	const bool bSaveSuccess = SaveSystem->SaveGame(false, *FullSavePath, PlayerIndex, SavedData);
	if (!bSaveSuccess)
	{
		UE_LOG(LogEasyMultiSave, Error, TEXT("Failed to save game data to: %s"), *FullSavePath);
	}

	return bSaveSuccess;
}

void UEMSObjectBase::PerformAutoBackup(const FString& FullSavePath) const
{
	const EFileValidity ValidCheck = IsSaveFileValid(FullSavePath);
	if (ValidCheck == EFileValidity::FILE_INVALID)
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Auto backup skipped due to invalid save file: %s"), *FullSavePath);
		return;
	}
	else if (ValidCheck == EFileValidity::FILE_MISSING)
	{
		return;
	}

	//Load existing save data
	TArray<uint8> BinaryData;
	if (!LoadBinaryData(FullSavePath, BinaryData))
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Auto backup failed, unable to load existing save: %s"), *FullSavePath);
		return;
	}

	const FString BackupPath = FSavePaths::GetBackupSavePath(FullSavePath);
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!SaveSystem->SaveGame(false, *BackupPath, PlayerIndex, BinaryData))
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("Auto backup failed to save: %s"), *BackupPath);
	}
}

bool UEMSObjectBase::SaveBinaryArchive(FBufferArchive& BinaryData, const FString& FullSavePath) const
{

#if EMS_PLATFORM_DESKTOP
	FSavePaths::CheckForReadOnly(FullSavePath);
#endif

	bool bSuccess = false;
	const bool bNoCompression = IsConsoleFileSystem();

	WriteGameVersionInfo(BinaryData);

	if (bNoCompression)
	{
		bSuccess = SaveBinaryData(BinaryData, FullSavePath);
	}
	else
	{
		//Compress and save
		TArray<uint8> CompressedData;
		FArchiveSaveCompressedProxy Compressor(CompressedData, NAME_Oodle);

		if (Compressor.GetError())
		{
			UE_LOG(LogEasyMultiSave, Error, TEXT("Cannot save, compressor error: %s"), *FullSavePath);
			return false;
		}

		Compressor << BinaryData;
		Compressor.Flush(); //Finalizes compression

		if (CompressedData.Num() == 0)
		{
			UE_LOG(LogEasyMultiSave, Error, TEXT("Compression failed: Data is empty after compression: %s"), *FullSavePath);
			return false;
		}

		bSuccess = SaveBinaryData(CompressedData, FullSavePath);
		if (!bSuccess)
		{
			UE_LOG(LogEasyMultiSave, Error, TEXT("Failed to save compressed data: %s"), *FullSavePath);
			return false;
		}

		Compressor.Close(); //Ensures compression finalization
	}

	BinaryData.Empty(); //Clears the buffer

	return bSuccess;
}

bool UEMSObjectBase::LoadBinaryData(const FString& FullSavePath, TArray<uint8>& OutBinaryData) const
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!SaveSystem->DoesSaveGameExist(*FullSavePath, PlayerIndex))
	{
		OutBinaryData.Empty();
		return false;
	}

	if (!SaveSystem->LoadGame(false, *FullSavePath, PlayerIndex, OutBinaryData))
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("%s could not be loaded"), *FullSavePath);
		OutBinaryData.Empty();
		return false;
	}

	if (EMS::ArrayEmpty(OutBinaryData))
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("No binary data found for %s"), *FullSavePath);
		OutBinaryData.Empty();
		return false;
	}

	return true;
}

bool UEMSObjectBase::LoadBinaryArchive(const EDataLoadType LoadType, const FString& FullSavePath, UObject* Object, const bool bReadVersion)
{
	TArray<uint8> BinaryData;
	if (!LoadBinaryData(FullSavePath, BinaryData))
	{
		return false;
	}

	bool bSuccess = false;
	const bool bNoCompression = IsConsoleFileSystem();

	// Eliminates duplication between the compressed and uncompressed code paths.
	const auto ReadFromArchive = [this, LoadType, Object, bReadVersion](const TArray<uint8>& Archive) -> bool
	{
		FMemoryReader MemoryReader(Archive, true);
		ReadPackageInfo(MemoryReader, true);

		const bool bSuccess = UnpackBinaryArchive(LoadType, MemoryReader, Object);

		if (bReadVersion)
		{
			ReadGameVersionInfo(MemoryReader);
		}

		return bSuccess;
	};

	if (bNoCompression)
	{
		bSuccess = ReadFromArchive(BinaryData);
	}
	else
	{
		FArchiveLoadCompressedProxy Decompressor = FArchiveLoadCompressedProxy(BinaryData, NAME_Oodle);

		if (Decompressor.GetError())
		{
			UE_LOG(LogEasyMultiSave, Error, TEXT("Cannot load, file might not be compressed: %s"), *FullSavePath);
			return false;
		}

		FBufferArchive DecompressedBinary;
		Decompressor << DecompressedBinary;

		bSuccess = ReadFromArchive(DecompressedBinary);

		//Proper cleanup
		Decompressor.Close();        //Close decompressor
		DecompressedBinary.Empty();  //Clear decompressed memory
	}

	return bSuccess;
}

/**
Base Serialize Functions
**/

void UEMSObjectBase::SerializeToBinary(UObject* Object, TArray<uint8>& OutData) const
{
	FMemoryWriter MemoryWriter(OutData, true);
	FSaveGameArchive Ar(MemoryWriter);
	Object->Serialize(Ar);

	//Write Multi-Level package tag
	if (FSaveVersion::RequiresPerObjectPackageTag(Object))
	{
		FSaveVersion::WriteObjectPackageTag(OutData);
	}
}

void UEMSObjectBase::SerializeFromBinary(UObject* Object, const TArray<uint8>& InData)
{
	FMemoryReader MemoryReader(InData, true);
	ReadPackageInfo(MemoryReader);

	//Check for Multi-Level package version tag
	if (FSaveVersion::RequiresPerObjectPackageTag(Object))
	{
		if (!FSaveVersion::CheckObjectPackageTag(InData))
		{
			//Without tag, we assume the old package version.
			const FPackageFileVersion OldPackage = FSaveVersion::GetStaticOldPackageVersion();
			MemoryReader.SetUEVer(OldPackage);
		}
	}

	FSaveGameArchive Ar(MemoryReader);
	Object->Serialize(Ar);
}

/**
Versioning Functions
**/

void UEMSObjectBase::WritePackageInfo(FBufferArchive& ToBinary) const
{
	//Package info is written at the beginning of the file as first entry to the top-level FBufferArchive for Player, Level, Object
	int32 FileTag = EMS::UE_SAVEGAME_FILE_TYPE_TAG;
	FPackageFileVersion Version = GPackageFileUEVersion;
	FEngineVersion EngineVersion = FEngineVersion::Current();

	ToBinary << FileTag;
	ToBinary << Version;
	ToBinary << EngineVersion;
}

void UEMSObjectBase::ReadPackageInfo(FMemoryReader& MemoryReader, const bool bSeekInitialVersion)
{
	//This is done once when initially reading the file
	if (bSeekInitialVersion)
	{
		int32 FileTag;
		FPackageFileVersion FileVersion;
		FEngineVersion EngineVersion;

		MemoryReader << FileTag;

		//No file tag means an old file.
		if (FileTag != EMS::UE_SAVEGAME_FILE_TYPE_TAG)
		{
			//Start from beginning
			MemoryReader.Seek(0);

			LoadedPackageVersion = FSaveVersion::GetStaticOldPackageVersion();
			LoadedEngineVersion = FEngineVersion();

			UE_LOG(LogEasyMultiSave, Warning, TEXT("File version empty. Using 'Old Save Package Version': %d"), LoadedPackageVersion.ToValue());
		}
		else
		{
			MemoryReader << FileVersion;
			MemoryReader << EngineVersion;

			LoadedPackageVersion = FileVersion;
			LoadedEngineVersion = EngineVersion;
		}
	}

	//Sub-archives also require the correct version to be set, so we use the initial version globally 
	MemoryReader.SetUEVer(LoadedPackageVersion);
	MemoryReader.SetEngineVer(LoadedEngineVersion);
}

void UEMSObjectBase::WriteGameVersionInfo(FBufferArchive& ToBinary) const
{
	//Game version info is written to the end of the file
	FSaveVersionInfo GameVersion = FSaveVersion::MakeSaveFileVersion();
	ToBinary << GameVersion;
}

void UEMSObjectBase::ReadGameVersionInfo(FMemoryReader& FromBinary)
{
	//Called from LoadBinaryArchive
	FromBinary << LastReadVersion;

	UE_LOG(LogEasyMultiSave, Log, TEXT("%s | Package: %d | Engine: %s | Plugin: %s | Game: %s"),
		*EMS::VersionLogText, LoadedPackageVersion.ToValue(), *LoadedEngineVersion.ToString(), *LastReadVersion.Plugin, *LastReadVersion.Game);
}

bool UEMSObjectBase::CheckSaveGameIntegrity(const EDataLoadType Type, const FString& FullSavePath, const bool bComplexCheck)
{
	bool bSuccess = false;
	const EFileValidity ValidCheck = IsSaveFileValid(FullSavePath);

	//Integrity check, logging handled internally
	if (ValidCheck == EFileValidity::FILE_VALID)
	{
		if (bComplexCheck)
		{
			//Use a fire and forget dummy object, to keep the check independent
			UEMSInfoSaveGame* DummyObject = Type == EDataLoadType::DATA_Object ? NewObject<UEMSInfoSaveGame>(GetTransientPackage()) : nullptr;
			bSuccess = LoadBinaryArchive(Type, FullSavePath, DummyObject, true);

			if (!FSaveVersion::IsSaveGameVersionEqual(LastReadVersion))
			{
				UE_LOG(LogEasyMultiSave, Warning, TEXT("Save Game Version mismatch: File: %s | Settings: %s"), *LastReadVersion.Game, *FSaveVersion::GetGameVersion());
				bSuccess = false;
			}
		}
	}

	//Make sure we are not loading memory-only data later
	ClearLoadFromMemory();

	//A missing file can return true, as there is nothing to check
	return ValidCheck != EFileValidity::FILE_INVALID;
}

/**
File System
**/

bool UEMSObjectBase::VerifyOrCreateDirectory(const FString& NewDir) const
{
	//Not required for console
	if (IsConsoleFileSystem())
	{
		return true;
	}

	const FString SaveFile = FPaths::Combine(BaseSaveDir(), NewDir);
	if (IFileManager::Get().DirectoryExists(*SaveFile))
	{
		return true;
	}

	return IFileManager::Get().MakeDirectory(*SaveFile, true);
}

FString UEMSObjectBase::SaveUserDir()
{
	return FPaths::ProjectSavedDir() + TEXT("SaveGames/Users/");
}

FString UEMSObjectBase::UserSubDir() const
{
	// Takes into account the already defined path from ISaveGame
	return TEXT("Users/") + GetCurrentSaveUserName() + EMS::Slash;
}

FString UEMSObjectBase::BaseSaveDir() const
{
	if (HasSaveUserName())
	{
		return SaveUserDir() + GetCurrentSaveUserName() + EMS::Slash;
	}

	return FPaths::ProjectSavedDir() + EMS::SaveGamesFolder;
}

FString UEMSObjectBase::ConsoleSaveDir()
{
	return FPaths::ProjectSavedDir() + EMS::SaveGamesFolder;
}

FString UEMSObjectBase::GetThumbnailFormat()
{
	return FSavePaths::GetThumbnailFormat();
}

FString UEMSObjectBase::GetThumbnailFileExtension()
{
	return FSavePaths::GetThumbnailFileExtension();
}

FString UEMSObjectBase::AllThumbnailFiles() const
{
	return BaseSaveDir() + TEXT("*") + GetThumbnailFileExtension();
}

FString UEMSObjectBase::GetFolderOrFile() const
{
	// Console uses file names and not folders, "/" will automatically create a new folder.
	const bool bFile = IsConsoleFileSystem();
	const FString FolderOrFile = bFile ? EMS::Underscore : EMS::Slash;

	return FolderOrFile;
}

FString UEMSObjectBase::FullSaveDir(const FString& DataType, FString SaveGameName) const
{
	if (SaveGameName.IsEmpty())
	{
		SaveGameName = GetCurrentSaveGameName();
	}

	// *.sav is added by ISaveInterface
	const FString FullName = SaveGameName + GetFolderOrFile() + DataType;

	if (HasSaveUserName())
	{
		return UserSubDir() + FullName;
	}

	return FullName;
}

FString UEMSObjectBase::CustomSaveFile(const FString& CustomSaveName, const FString& SlotName) const
{
	// Bound to a save slot, use default dir.
	if (!SlotName.IsEmpty())
	{
		return FullSaveDir(CustomSaveName, SlotName);
	}

	// Not bound to slot, so we just save in the base folder. with user if desired.
	if (HasSaveUserName())
	{
		return UserSubDir() + CustomSaveName;
	}

	return CustomSaveName;
}

FString UEMSObjectBase::SlotInfoSaveFile(const FString& SaveGameName) const
{
	return FullSaveDir(EMS::SlotSuffix, SaveGameName);
}

FString UEMSObjectBase::ActorSaveFile(const FString& SaveGameName) const
{
	return FullSaveDir(EMS::ActorSuffix, SaveGameName);
}

FString UEMSObjectBase::PlayerSaveFile(const FString& SaveGameName) const
{
	return FullSaveDir(EMS::PlayerSuffix, SaveGameName);
}

FString UEMSObjectBase::ThumbnailSaveFile(const FString& SaveGameName) const
{
	const FString ThumbnailPath = BaseSaveDir() + SaveGameName + GetFolderOrFile();
	return ThumbnailPath + EMS::ThumbSuffix + GetThumbnailFileExtension();
}

FString UEMSObjectBase::SlotFilePath(const FString& SaveGameName) const
{
	// This is only used for sorting.
	return BaseSaveDir() + SlotInfoSaveFile(SaveGameName) + EMS::SaveType;
}

/**
Thumbnail Saving
Export from a 2d scene capture render target source.
**/

UTexture2D* UEMSObjectBase::ImportSaveThumbnail(const FString& SaveGameName)
{
	const FString SaveThumbnailName = ThumbnailSaveFile(SaveGameName);

	//Suppress warning messages when we dont have a thumb yet.
	if (FPaths::FileExists(SaveThumbnailName))
	{
		return FImageUtils::ImportFileAsTexture2D(SaveThumbnailName);
	}

	return nullptr;
}

void UEMSObjectBase::ExportSaveThumbnail(UTextureRenderTarget2D* TextureRenderTarget, const FString& SaveGameName)
{
	if (!TextureRenderTarget)
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("ExportSaveThumbnailRT: TextureRenderTarget must be non-null"));
	}
	else if (!FSaveThumbnails::HasRenderTargetResource(TextureRenderTarget))
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("ExportSaveThumbnailRT: Render target has been released"));
	}
	else if (SaveGameName.IsEmpty())
	{
		UE_LOG(LogEasyMultiSave, Warning, TEXT("ExportSaveThumbnailRT: FileName must be non-empty"));
	}
	else
	{
		const FString SaveThumbnailName = ThumbnailSaveFile(SaveGameName);
		const bool bSuccess = FSaveThumbnails::ExportRenderTarget(TextureRenderTarget, SaveThumbnailName);

		if (!bSuccess)
		{
			UE_LOG(LogEasyMultiSave, Warning, TEXT("ExportSaveThumbnailRT: FileWrite failed to create"));
		}
	}
}

