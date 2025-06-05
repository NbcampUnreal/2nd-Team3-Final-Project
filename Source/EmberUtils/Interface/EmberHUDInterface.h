#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "EmberHUDInterface.generated.h"

UINTERFACE()
class UEmberHUDInterface : public UInterface
{
	GENERATED_BODY()
};

class EMBERUTILS_API IEmberHUDInterface
{
	GENERATED_BODY()

public:
	virtual bool RegisterLayer(const FGameplayTag& LayerTag, UUserWidget* Layer) = 0;

	/** 해당 레이어에 위젯을 Push (즉시 반환할 때는 생성된 UUserWidget*) */
	virtual UUserWidget* PushContentToLayer(const FGameplayTag& LayerTag, const TSubclassOf<UUserWidget>& WidgetClass) = 0;

	/** 해당 레이어에서 가장 상단 위젯을 Pop */
	virtual void PopContentToLayer(const FGameplayTag& LayerTag) = 0;

	/** 해당 레이어의 모든 위젯을 제거 */
	virtual void ClearToLayer(const FGameplayTag& LayerTag) = 0;

	/** 좌클릭 입력 잠금(Get) */
	virtual bool GetGameLeftMouseInputLock() const = 0;

	/** 좌클릭 입력 잠금(Set) */
	virtual void SetGameLeftMouseInputLock(bool bLock) = 0;

	/** 이동 입력 잠금(Get) */
	virtual bool GetGameMovementInputLock() const = 0;

	/** 이동 입력 잠금(Set) */
	virtual void SetGameMovementInputLock(bool bLock) = 0;
};
