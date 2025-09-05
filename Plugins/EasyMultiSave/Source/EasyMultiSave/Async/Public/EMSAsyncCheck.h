//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#pragma once

#include "CoreMinimal.h"
#include "EMSData.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EMSAsyncCheck.generated.h"

class UEMSObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCheckCompletedPin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCheckFailedPin);

UCLASS()
class EASYMULTISAVE_API UEMSAsyncCheck : public UBlueprintAsyncActionBase
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FCheckCompletedPin OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FCheckFailedPin OnFailed;

private:

	UPROPERTY()
	TObjectPtr<UEMSObject> EMS;

	ESaveFileCheckType Type;
	FString SaveFileName;

	uint8 bCheckSuccess : 1;
	uint8 bCheckGameVersion : 1;

public:

	/**
	* Check the integrity of available files for the current Save Slot. See log for further output information.
	* 
	* @param CheckType - The type of integrity check to perform.
	* @param CustomSaveName - Only relevant when checking for a Custom Save Game.
	* @param bComplexCheck - Loads the complete data and compares save files against 'Save Game Version' from the plugin settings. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Files", meta = (DisplayName = "Check Save File Integrity", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AdvancedDisplay = "CustomSaveName, bComplexCheck"))
	static UEMSAsyncCheck* CheckSaveFiles(UObject* WorldContextObject, ESaveFileCheckType CheckType, FString CustomSaveName, bool bComplexCheck);

	virtual void Activate() override;

private:

	void StartCheck();
	void CheckPlayer();
	void CheckLevel();
	void CheckCustom();
	void CheckCustomSlot();
	void CompleteCheck();
};
