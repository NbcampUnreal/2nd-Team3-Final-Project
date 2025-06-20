// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveManagerSubsystem.generated.h"

class USaveManagerData;

/**
 * Subsystem for managing save slots and arbitrary data storage using Easy Multi Save.
 * Provides simple key-value Save/Load functions.
 * Replace MYGAME_API with your project's API macro.
 */
UCLASS()
class EMBERUTILS_API USaveManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    /** Set the active save slot name. */
    UFUNCTION(BlueprintCallable, Category = "Save")
    void SetSaveSlotName(const FString& SlotName);

    /** Save basic types via key (int, float, bool, string) */
    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveInt(const FString& Key, int32 Value);
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool LoadInt(const FString& Key, int32& OutValue) const;

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveFloat(const FString& Key, float Value);
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool LoadFloat(const FString& Key, float& OutValue) const;

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveBool(const FString& Key, bool Value);
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool LoadBool(const FString& Key, bool& OutValue) const;

    UFUNCTION(BlueprintCallable, Category = "Save")
    void SaveString(const FString& Key, const FString& Value);
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool LoadString(const FString& Key, FString& OutValue) const;

    /** Generic Save/Load for structs (JSON-serialized). C++ only. */
    template<typename StructType>
    void SaveStruct(const FString& Key, const StructType& Value);

    template<typename StructType>
    bool LoadStruct(const FString& Key, StructType& OutValue) const;

    /** Check if a key exists in the saved data. */
    UFUNCTION(BlueprintCallable, Category = "Save")
    bool ContainsKey(const FString& Key) const;

    /** Delete all save data for the current slot. */
    UFUNCTION(BlueprintCallable, Category = "Save")
    void DeleteSaveSlot();

private:
    /** Helper to retrieve or create the custom save object. */
    USaveManagerData* GetSaveData() const;

    /** Current EMS save slot name. */
    FString CurrentSlotName;

    /** Fixed file name for the custom save object. */
    static const FString SaveFileName;
};