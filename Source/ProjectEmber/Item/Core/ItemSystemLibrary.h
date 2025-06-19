// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "ItemSystemLibrary.generated.h"

struct FActiveGameplayEffectHandle;
class UAbilitySystemComponent;
class UItemSubsystem;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UItemSystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Item|Function")
	static UGameInstance* GetGameInstance();
	UFUNCTION(BlueprintPure, Category = "Item System|Subsystem")
	static UItemSubsystem* GetItemSubsystem();

	UFUNCTION(BlueprintCallable, Category = "Item System|GAS", meta=(DisplayName="Apply Item Effect Infos"))
	static TArray<FActiveGameplayEffectHandle> ApplyEffectInfoList(UAbilitySystemComponent* TargetASC, const TArray<FItemEffectApplicationInfo>& EffectInfos, UObject* SourceObject, float SourceLevel = 1.0f);

	template <typename TRowType>
	static const TRowType* FindComponentDataRowInHandles(const TArray<FDataTableRowHandle>& ComponentHandles, const FString& ContextString = TEXT(""));
};


template <typename TRowType>
const TRowType* UItemSystemLibrary::FindComponentDataRowInHandles(const TArray<FDataTableRowHandle>& ComponentHandles, const FString& ContextString)
{
	for (const FDataTableRowHandle& Handle : ComponentHandles)
	{
		if (!Handle.IsNull() && Handle.DataTable) 
		{
			const TRowType* FoundRow = Handle.GetRow<TRowType>(ContextString);
			if (FoundRow)
			{
				return FoundRow;
			}
		}
	}

	return nullptr;
}