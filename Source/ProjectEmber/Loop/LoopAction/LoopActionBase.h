// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Loop/Data/LoopEventDataAsset.h"
#include "LoopActionBase.generated.h"

class ULoopEventDirector;
class ULoopActionFragment;
class ULoopEventDataAsset;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class PROJECTEMBER_API ULoopActionBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ActivateAction();

	void Initialize(ULoopEventDirector* InDirector, ULoopActionDefinition* InDefinition);
	
	UFUNCTION(BlueprintCallable)
	void FinishAction();
	
	UFUNCTION(BlueprintCallable, Category = "Helpers")
	ULoopActionFragment* FindFragmentByClass(TSubclassOf<ULoopActionFragment> FragmentClass) const;

	UFUNCTION(BlueprintCallable, Category = "Helpers")
	ACharacter* GetPlayerCharacterFromWorld();
protected:
	template<typename T>
	T* FindFragment() const
	{
		for (ULoopActionFragment* Frag : Definition->Fragments)
		{
			if (T* Casted = Cast<T>(Frag))
			{
				return Casted;
			}
		}
		return nullptr;
	}
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULoopEventDirector> OwnerDirector;
	
	UPROPERTY()
	TObjectPtr<ULoopActionDefinition> Definition;
};

