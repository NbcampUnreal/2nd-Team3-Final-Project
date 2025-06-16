#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "InputCoreTypes.h"
#include "EmberKeyMappingEntry.generated.h"

UENUM(BlueprintType)
enum class EMoveDirection : uint8
{
    Unknown   UMETA(DisplayName = "Unknown"),
    Forward   UMETA(DisplayName = "Forward"),
    Backward  UMETA(DisplayName = "Backward"),
    Left      UMETA(DisplayName = "Left"),
    Right     UMETA(DisplayName = "Right")
};

USTRUCT(BlueprintType)
struct FEmberKeyMappingEntry
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MappingName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<const UInputAction> InputAction = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FKey BoundKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FKey DefaultKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName MappableKeyID; // ¡Ú Ãß°¡!

    FEmberKeyMappingEntry() : InputAction(nullptr), BoundKey(EKeys::Invalid) {}
};

USTRUCT(BlueprintType)
struct FEmberDirectionalMoveEntry
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMoveDirection Direction = EMoveDirection::Unknown;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FKey BoundKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FKey DefaultKey;

    FEmberDirectionalMoveEntry(): Direction(EMoveDirection::Unknown), BoundKey(EKeys::Invalid), DefaultKey(EKeys::Invalid){}
};