//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#pragma once

#include "EMSData.h"
#include "EMSPluginSettings.h"
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Misc/EngineVersion.h"
#include "EMSObjectBase.generated.h"

class UWorld;
class UTextureRenderTarget2D;
class UTexture2D;
class UEMSInfoSaveGame;
class UEMSCustomSaveGame;
class FBufferArchive;
class FMemoryReader;
class FMemoryWriter;

UCLASS(config = EmsUser, configdonotcheckdefaults, abstract, NotBlueprintType, NotBlueprintable)
class EASYMULTISAVE_API UEMSObjectBase : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	UEMSObjectBase();

/** Variables  */

protected:

	const uint32 PlayerIndex = 0;

	uint8 bLoadFromMemory : 1;

private:

	FSaveVersionInfo LastReadVersion;
	FPackageFileVersion LoadedPackageVersion;
	FEngineVersion LoadedEngineVersion;

private:

	UPROPERTY(config)
	FString CurrentSaveGameName;

	UPROPERTY(config)
	FString CurrentSaveUserName;

	float LastSlotSaveTime = 0.f;

	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UEMSInfoSaveGame>> CachedSaveSlots;

	UPROPERTY(Transient)
	TMap<FString, TObjectPtr<UEMSCustomSaveGame>> CachedCustomSaves;

/** Default Implementations  */

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UWorld* GetWorld() const override;
	const bool HasValidWorld() const;

	virtual bool UnpackBinaryArchive(const EDataLoadType LoadType, FMemoryReader& FromBinary, UObject* Object) { return false; }

	FORCEINLINE void ClearLoadFromMemory()
	{
		//This is only used in EMSObject, but implemented here
		bLoadFromMemory = false;
	}

/** Save Object Creation  */

private:

	template <class TSaveGame>
	TSaveGame* CreateNewSaveObject(const FString& FullSavePath, const FSoftClassPath& InClassName);

/** Custom Saves  */

public:

	UEMSCustomSaveGame* GetCustomSave(const TSubclassOf<UEMSCustomSaveGame>& SaveGameClass, const FString& InSlotName, const FString& InFileName);
	bool SaveCustom(UEMSCustomSaveGame* SaveGame);
	bool SaveAllCustomObjects();
	void DeleteCustomSave(UEMSCustomSaveGame* SaveGame);

private:

	void ClearCachedCustomSaves();
	void ClearCustomSavesDesktop(const FString& SaveGameName);
	void ClearCustomSavesConsole(const FString& SaveGameName, const bool bAddFiles, TArray<FString>& OutFiles);

/** Save Slot and User Names  */

public:

	FString GetCurrentSaveGameName() const;
	void SetCurrentSaveGameName(const FString& SaveGameName);
	TArray<FString> GetSortedSaveSlots() const;

	UEMSInfoSaveGame* GetSlotInfoObject(const FString& SaveGameName = FString());
	TArray<FSaveSlotInfo> GetSlotInfos(const TArray<FString>& SaveGameNames) const;
	TArray<FString> GetAllSaveGames() const;

	bool DoesSaveGameExist(const FString& SaveGameName) const;
	bool DoesFullSaveGameExist(const FString& SaveGameName) const;
	void DeleteAllSaveDataForSlot(const FString& SaveGameName);
	void SaveSlotInfoObject(const FString& SaveGameName);

	FString GetCurrentSaveUserName() const;
	void SetCurrentSaveUserName(const FString& UserName);
	void DeleteAllSaveDataForUser(const FString& UserName);
	TArray<FString> GetAllSaveUsers() const;

protected:

	bool HasSaveUserName() const;
	UEMSInfoSaveGame* MakeSlotInfoObject(const FString& SaveGameName = FString());
	void ClearCachedSlots();
	virtual void ClearUserData();

/** Save and Load Archive Functions  */

protected:

	bool SaveObject(const FString& FullSavePath, UObject* SaveGameObject) const;

	bool LoadBinaryData(const FString& FullSavePath, TArray<uint8>& OutBinaryData) const;
	bool SaveBinaryData(const TArray<uint8>& SavedData, const FString& FullSavePath) const;
	void PerformAutoBackup(const FString& FullSavePath) const;

	bool SaveBinaryArchive(FBufferArchive& BinaryData, const FString& FullSavePath) const;
	bool LoadBinaryArchive(const EDataLoadType LoadType, const FString& FullSavePath, UObject* Object = nullptr, const bool bReadVersion = false);

/** Base Serialize Functions  */

protected:

	void SerializeToBinary(UObject* Object, TArray<uint8>& OutData) const;
	void SerializeFromBinary(UObject* Object, const TArray<uint8>& InData);

/** Versioning Functions  */

public:

	bool CheckSaveGameIntegrity(const EDataLoadType Type, const FString& FullSavePath, const bool bComplexCheck);

protected:

	void ReadGameVersionInfo(FMemoryReader& FromBinary);
	void WriteGameVersionInfo(FBufferArchive& ToBinary) const;
	void WritePackageInfo(FBufferArchive& ToBinary) const;
	void ReadPackageInfo(FMemoryReader& MemoryReader, const bool bSeekInitialVersion = false);

/** File System and Path Names  */

public:

	FString SlotInfoSaveFile(const FString& SaveGameName = FString()) const;
	FString CustomSaveFile(const FString& CustomSaveName, const FString& SlotName) const;
	FString ActorSaveFile(const FString& SaveGameName = FString()) const;
	FString PlayerSaveFile(const FString& SaveGameName = FString())  const;

protected:

	bool VerifyOrCreateDirectory(const FString& NewDir) const;

	static FString SaveUserDir();
	FString UserSubDir() const;
	FString BaseSaveDir() const;
	static FString ConsoleSaveDir();
	static FString GetThumbnailFormat();
	static FString GetThumbnailFileExtension();

	FString AllThumbnailFiles() const;
	FString GetFolderOrFile() const;
	FString FullSaveDir(const FString& DataType, FString SaveGameName = FString()) const;

	FString ThumbnailSaveFile(const FString& SaveGameName) const;
	FString SlotFilePath(const FString& SaveGameName = FString()) const;

/** Thumbnails  */

public:

	UTexture2D* ImportSaveThumbnail(const FString& SaveGameName);
	void ExportSaveThumbnail(UTextureRenderTarget2D* TextureRenderTarget, const FString& SaveGameName);

/** Settings Helpers  */

public:

	FORCEINLINE static bool IsNormalMultiLevelSave()
	{
		return FSettingHelpers::IsNormalMultiLevelSave();
	}

	FORCEINLINE static bool IsStreamMultiLevelSave()
	{
		return FSettingHelpers::IsStreamMultiLevelSave();
	}

	FORCEINLINE static bool IsFullMultiLevelSave()
	{
		return FSettingHelpers::IsFullMultiLevelSave();
	}

	FORCEINLINE static bool IsStackBasedMultiLevelSave()
	{
		return FSettingHelpers::IsStackBasedMultiLevelSave();
	}

	FORCEINLINE static bool IsContainingStreamMultiLevelSave()
	{
		return FSettingHelpers::IsContainingStreamMultiLevelSave();
	}

	FORCEINLINE static bool IsConsoleFileSystem()
	{
		return FSettingHelpers::IsConsoleFileSystem();
	}

	FORCEINLINE static bool IsPersistentGameMode()
	{
		return FSettingHelpers::IsPersistentGameMode();
	}

	FORCEINLINE static bool IsPersistentPlayer()
	{
		return FSettingHelpers::IsPersistentPlayer();
	}

	FORCEINLINE static bool IsMultiThreadLoading()
	{
		return FSettingHelpers::IsMultiThreadLoading();
	}

	FORCEINLINE static bool IsDeferredLoading()
	{
		return FSettingHelpers::IsDeferredLoading();
	}

/** Save Helpers  */

public:

	FORCEINLINE FName GetLevelName() const
	{
		return FActorHelpers::GetWorldLevelName(GetWorld());
	}

	FORCEINLINE	static TArray<uint8> BytesFromString(const FString& String)
	{
		return FSaveHelpers::BytesFromString(String);
	}

	FORCEINLINE static FString StringFromBytes(const TArray<uint8>& Bytes)
	{
		return FSaveHelpers::StringFromBytes(Bytes);
	}

	FORCEINLINE static bool CompareIdentifiers(const TArray<uint8>& ArrayId, const FString& StringId)
	{
		return FSaveHelpers::CompareIdentifiers(ArrayId, StringId);
	}

	FORCEINLINE bool IsAsyncSaveOrLoadTaskActive(const ESaveGameMode Mode = ESaveGameMode::MODE_All, const EAsyncCheckType CheckType = EAsyncCheckType::CT_Both, const bool bLog = true) const
	{
		return FAsyncSaveHelpers::IsAsyncSaveOrLoadTaskActive(GetWorld(), Mode, CheckType, bLog);
	}

	FORCEINLINE static EFileValidity IsSaveFileValid(const FString& FullSavePath, const bool bLog = true)
	{
		return FSaveVersion::IsSaveFileValid(FullSavePath, bLog);
	}

	FORCEINLINE bool IsPaused() const
	{
		return GetWorld()->IsPaused();	
	}

};
