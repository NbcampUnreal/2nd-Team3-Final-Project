#pragma once

#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

namespace EmberGameplayTag
{
	EMBERUTILS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Character_AttackHit);
};

namespace EmberQuestEventTag
{
	EMBERUTILS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Following);
	EMBERUTILS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Shooting);
	EMBERUTILS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ring);
	EMBERUTILS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Raid);
};

namespace EmberInteractionEventTag
{
	EMBERUTILS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Clear);
}