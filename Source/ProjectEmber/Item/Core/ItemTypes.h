// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "StructUtils/InstancedStruct.h"
#include "ItemTypes.generated.h"

/**
 * RowName은 ItemID와 동일하게 사용
 */
class UTexture2D;
class USoundBase;
class UGameplayEffect;



// 기본 아이템 정보
USTRUCT(BlueprintType)
struct FItemMasterInfoRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
    FText ItemDisplayName = FText();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
    FText ItemDescription = FText();
    
    // 아이템의 기능적/데이터적 컴포넌트(특성)들을 정의하는 다른 데이터 테이블 행들을 참조
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemMasterInfoRow")
    TArray<FDataTableRowHandle> ItemData;
};

USTRUCT(BlueprintType)
struct FSlotInfoRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SlotInfo")
    TSoftObjectPtr<UTexture2D> ItemIcon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SlotInfo", meta = (ClampMin = "1"))
    int32 MaxStackSize = 1;

    void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const
    {
        OutInstancedStruct.InitializeAs<FSlotInfoRow>(*this);
    }
};


// GAS 효과 적용 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FItemEffectApplicationInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName EffectName = FName();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName EffectDetail = FName();
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UGameplayEffect> GameplayEffectClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Gameplay.Effect.SetByCaller"))
    FGameplayTag MagnitudeSetByCallerTag = FGameplayTag::EmptyTag;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Magnitude = 0.0f;

    void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const
    {
        OutInstancedStruct.InitializeAs<FItemEffectApplicationInfo>(*this);
    }
};

// 소비 컴포넌트 정보 (DT_ConsumableComponent 용)
USTRUCT(BlueprintType)
struct FConsumableInfoRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable Component", meta = (ClampMin = "0"))
    int32 ConsumeAmount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable Component")
    TSubclassOf<UGameplayEffect> CooldownGameplayEffect = nullptr;

    // 소비 시 적용될 효과 목록 (GAS 연동)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable Component")
    TArray<FItemEffectApplicationInfo> EffectsToApplyOnConsume;
    
    
    void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const
    {
        OutInstancedStruct.InitializeAs<FConsumableInfoRow>(*this);
    }
};

// 장비 컴포넌트 정보 (DT_ConsumableComponent 용)
USTRUCT(BlueprintType)
struct FEquipmentInfoRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment Component")
    FGameplayTagContainer EquipmentTag = FGameplayTagContainer::EmptyContainer;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment Component")
    TArray<FDataTableRowHandle> MainEffects;

    void InitializeInstancedStruct(FInstancedStruct& OutInstancedStruct) const
    {
        OutInstancedStruct.InitializeAs<FEquipmentInfoRow>(*this);
    }
};

