#pragma once

#include "CoreMinimal.h"
#include "EMSActorSaveInterface.h"
#include "AttributeSaveData.generated.h"

USTRUCT(BlueprintType)
struct EMBERABILITYSYSTEM_API FEmberCharacterAttributeSetSaveData
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame)
	float AttackRate;

	UPROPERTY(SaveGame)
	float MaxAttackRate;

	UPROPERTY(SaveGame)
	float Health;

	UPROPERTY(SaveGame)
	float MaxHealth;

	UPROPERTY(SaveGame)
	float Damage;

	UPROPERTY(SaveGame)
	float Mana;

	UPROPERTY(SaveGame)
	float MaxMana;

	UPROPERTY(SaveGame)
	float Shield;

	UPROPERTY(SaveGame)
	float MaxShield;
	
	FEmberCharacterAttributeSetSaveData()
		: AttackRate(0.f)
		, MaxAttackRate(0.f)
		, Health(0.f)
		, MaxHealth(0.f)
		, Damage(0.f)
		, Mana(0.f)
		, MaxMana(0.f)
		, Shield(0.f)
		, MaxShield(0.f)
	{}
};
