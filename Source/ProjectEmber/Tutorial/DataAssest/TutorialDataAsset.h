// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TutorialDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FTutorialData
{
    GENERATED_BODY()

public:
    FTutorialData()
        : Name(FText::FromString(TEXT("Default Name")))
        , Description(FText::FromString(TEXT("Default Description")))
        , KeyImage(nullptr)
        , Sound(nullptr)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* KeyImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* Sound;
};


UCLASS()
class PROJECTEMBER_API UTutorialDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FTutorialData> Tutorials;
};
