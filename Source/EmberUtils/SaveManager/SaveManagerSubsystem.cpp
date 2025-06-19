#include "SaveManagerSubsystem.h"
#include "SaveManagerData.h"
#include "EMSObject.h"
#include "EMSFunctionLibrary.h"
#include "JsonObjectConverter.h"

// Define the custom save file name
const FString USaveManagerSubsystem::SaveFileName = TEXT("SaveManagerData");

void USaveManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    // Default slot name (replace with your own if needed)
    CurrentSlotName = TEXT("DefaultSaveSlot");
    // Inform EMS of the active slot
    UEMSFunctionLibrary::SetCurrentSaveGameName(this, CurrentSlotName);
}

void USaveManagerSubsystem::SetSaveSlotName(const FString& SlotName)
{
    CurrentSlotName = SlotName;
    UEMSFunctionLibrary::SetCurrentSaveGameName(this, CurrentSlotName);
}

USaveManagerData* USaveManagerSubsystem::GetSaveData() const
{
    // Get or create the custom save object for this slot
    UEMSObject* EMS = UEMSObject::Get(GetWorld());
    if (!EMS)
    {
        UE_LOG(LogTemp, Warning, TEXT("EMSObject not found."));
        return nullptr;
    }
    UEMSCustomSaveGame* Base = EMS->GetCustomSave(USaveManagerData::StaticClass(), CurrentSlotName, SaveFileName);
    return Cast<USaveManagerData>(Base);
}

void USaveManagerSubsystem::SaveString(const FString& Key, const FString& Value)
{
    USaveManagerData* Data = GetSaveData();
    if (Data)
    {
        Data->DataMap.Add(Key, Value);
        UEMSObject::Get(GetWorld())->SaveCustom(Data);
    }
}

bool USaveManagerSubsystem::LoadString(const FString& Key, FString& OutValue) const
{
    // Retrieve without modifying
    UEMSObject* EMS = UEMSObject::Get(GetWorld());
    if (!EMS) return false;
    UEMSCustomSaveGame* Base = EMS->GetCustomSave(USaveManagerData::StaticClass(), CurrentSlotName, SaveFileName);
    if (USaveManagerData* Data = Cast<USaveManagerData>(Base))
    {
        if (const FString* Found = Data->DataMap.Find(Key))
        {
            OutValue = *Found;
            return true;
        }
    }
    return false;
}

void USaveManagerSubsystem::SaveInt(const FString& Key, int32 Value)
{
    SaveString(Key, FString::FromInt(Value));
}

bool USaveManagerSubsystem::LoadInt(const FString& Key, int32& OutValue) const
{
    FString Str;
    if (LoadString(Key, Str))
    {
        OutValue = FCString::Atoi(*Str);
        return true;
    }
    return false;
}

void USaveManagerSubsystem::SaveFloat(const FString& Key, float Value)
{
    SaveString(Key, FString::SanitizeFloat(Value));
}

bool USaveManagerSubsystem::LoadFloat(const FString& Key, float& OutValue) const
{
    FString Str;
    if (LoadString(Key, Str))
    {
        OutValue = FCString::Atof(*Str);
        return true;
    }
    return false;
}

void USaveManagerSubsystem::SaveBool(const FString& Key, bool Value)
{
    SaveString(Key, Value ? TEXT("true") : TEXT("false"));
}

bool USaveManagerSubsystem::LoadBool(const FString& Key, bool& OutValue) const
{
    FString Str;
    if (LoadString(Key, Str))
    {
        OutValue = Str.Equals(TEXT("true"));
        return true;
    }
    return false;
}

template<typename StructType>
void USaveManagerSubsystem::SaveStruct(const FString& Key, const StructType& Value)
{
    FString Json;
    if (FJsonObjectConverter::UStructToJsonObjectString(Value, Json, 0, 0))
    {
        SaveString(Key, Json);
    }
}

template<typename StructType>
bool USaveManagerSubsystem::LoadStruct(const FString& Key, StructType& OutValue) const
{
    FString Json;
    if (LoadString(Key, Json))
    {
        return FJsonObjectConverter::JsonObjectStringToUStruct(Json, &OutValue, 0, 0);
    }
    return false;
}

bool USaveManagerSubsystem::ContainsKey(const FString& Key) const
{
    UEMSObject* EMS = UEMSObject::Get(GetWorld());
    if (!EMS) return false;
    UEMSCustomSaveGame* Base = EMS->GetCustomSave(USaveManagerData::StaticClass(), CurrentSlotName, SaveFileName);
    if (USaveManagerData* Data = Cast<USaveManagerData>(Base))
    {
        return Data->DataMap.Contains(Key);
    }
    return false;
}

void USaveManagerSubsystem::DeleteSaveSlot()
{
    UEMSFunctionLibrary::DeleteAllSaveDataForSlot(this, CurrentSlotName);
}
