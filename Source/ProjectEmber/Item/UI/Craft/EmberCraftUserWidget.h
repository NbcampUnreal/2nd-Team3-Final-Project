// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmberCraftUserWidget.generated.h"

class AEmberCharacter;
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UEmberCraftUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetCraftComponent(AEmberCharacter* InEmberCharacter);
	
	UFUNCTION(BlueprintCallable)
	void CrateItem(const FName& ItemName);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AEmberCharacter> EmberCharacter;
};
