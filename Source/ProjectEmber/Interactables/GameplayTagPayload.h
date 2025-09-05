// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayTagPayload.generated.h"

// 상호작용 액터 전용 텍스트 매크로 -> 위치 아직 못정해서 여기 뒀어요
namespace InteractionMessages
{
	constexpr TCHAR MessageType[] = TEXT("Interaction");
}
/**
 * 
 */
UCLASS()
class PROJECTEMBER_API UGameplayTagPayload : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag GameplayTag;
	
	UPROPERTY(BlueprintReadWrite)
	AActor* ContextObject;
};
