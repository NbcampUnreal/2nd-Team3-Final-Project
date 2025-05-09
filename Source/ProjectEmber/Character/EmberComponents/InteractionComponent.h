#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction")
	TScriptInterface<IInteractable> CurrentInteractable;

	// 상호작용 가능한 액터 오버랩 이벤트 발생시 인터페이스 함수 구현부 저장
	UFUNCTION()
	void SetCurrentInteractable(UObject* NewInteractable);

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void Interact();
};
