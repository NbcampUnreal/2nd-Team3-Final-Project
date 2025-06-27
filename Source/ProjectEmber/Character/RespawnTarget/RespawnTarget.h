#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "RespawnTarget.generated.h"

/* 
 * 리스폰시 부활할 TargetPoint 래핑클래스
 */
UCLASS()
class PROJECTEMBER_API ARespawnTarget : public ATargetPoint
{
	GENERATED_BODY()

public:
	ARespawnTarget();
	
};
