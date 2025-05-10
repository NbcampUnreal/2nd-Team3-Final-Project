// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "ItemTypes.generated.h"

/**
 * RowName은 ItemID와 동일하게 사용
 */
class UTexture2D;
class USoundBase;
class UGameplayEffect;

// 기본 아이템 정보
USTRUCT(BlueprintType)
struct FItemBaseInfoRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Base Info")
    FText ItemDisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Component")
    FText ItemDescription;
    
    // 아이템의 기능적/데이터적 컴포넌트(특성)들을 정의하는 다른 데이터 테이블 행들을 참조
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Base Info")
    TArray<FDataTableRowHandle> ItemComponentHandles;
};

USTRUCT(BlueprintType)
struct FInventoryComponentRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Component")
    TSoftObjectPtr<UTexture2D> ItemIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Component", meta = (ClampMin = "1"))
    int32 MaxStackSize = 1;

    // (선택 사항) 인벤토리 정렬 순서, 무게 등
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Base Info")
    TArray<FDataTableRowHandle> ItemComponentHandles;
};


// GAS 효과 적용 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FItemEffectApplicationInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> GameplayEffectClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Gameplay.Effect.SetByCaller"))
    FGameplayTag MagnitudeSetByCallerTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Magnitude = 0.0f;

};

// 소비 컴포넌트 정보 (DT_ConsumableComponent 용)
USTRUCT(BlueprintType)
struct FConsumableComponentRow : public FTableRowBase
{
    GENERATED_BODY()

    // 최대 사용 횟수 (0 또는 1은 보통 1회 사용 후 소멸)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable Component", meta = (ClampMin = "0"))
    int32 MaxCharges = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable Component")
    TSubclassOf<UGameplayEffect> CooldownGameplayEffect;

    // 소비 시 재생할 사운드 (선택 사항)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable Component")
    TSoftObjectPtr<USoundBase> ConsumeSound;

    // 소비 시 적용될 효과 목록 (GAS 연동)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable Component")
    TArray<FItemEffectApplicationInfo> EffectsToApplyOnConsume;
};