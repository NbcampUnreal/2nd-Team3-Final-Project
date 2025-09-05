//Easy Multi Save - Copyright (C) 2025 by Michael Hegemann.  

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/EngineTypes.h"
#include "EMSData.h"
#include "EMSActorSaveInterface.generated.h"

UINTERFACE(Category = "Easy Multi Save", BlueprintType, meta = (DisplayName = "EMS Actor Save Interface"))
class EASYMULTISAVE_API UEMSActorSaveInterface : public UInterface
{
	GENERATED_BODY()
};

class EASYMULTISAVE_API IEMSActorSaveInterface
{
	GENERATED_BODY()

public:
	
	/**Executed before the Actor and all of it's components are saved.*/
	UFUNCTION(BlueprintNativeEvent, Category = "Easy Multi Save")
	void ActorPreSave();

	/**Executed when the Actor and all of it's components have been saved.*/
	UFUNCTION(BlueprintNativeEvent, Category = "Easy Multi Save")
	void ActorSaved();

	/**Executed right before the Actor and all of it's components are loaded.*/
	UFUNCTION(BlueprintNativeEvent, Category = "Easy Multi Save")
	void ActorPreLoad();

	/**Executed after the Actor and all of it's components have been loaded.*/
	UFUNCTION(BlueprintNativeEvent, Category = "Easy Multi Save")
	void ActorLoaded();

	/**
	* (Not Level Blueprints) Holds the array of components that you want to save for the Actor.
	* This function is not relevant for Level Blueprints, as they cannot have components.
	*
	* @param Components - The Components that you want to save with the Actor.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Easy Multi Save")
	void ComponentsToSave(TArray<UActorComponent*>& Components);

};

