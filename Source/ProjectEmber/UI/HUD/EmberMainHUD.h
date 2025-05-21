#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayTagContainer.h"
#include "EmberMainHUD.generated.h"

class ULayerDebugger;
class UEmberLayerBase;

UCLASS(Blueprintable, BlueprintType)
class PROJECTEMBER_API AEmberMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	bool RegisterLayer(FGameplayTag LayerTag, UEmberLayerBase* Layer);

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void PushInitialWidget();

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	UUserWidget* PushContentToLayer(FGameplayTag LayerTag, const TSubclassOf<UUserWidget>& WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void PopContentToLayer(FGameplayTag LayerTag);

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void ClearToLayer(FGameplayTag LayerTag);

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetGameLeftMouseInputLock(bool bLock);

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	bool GetGameLeftMouseInputLock();

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void SetGameMovementInputLock(bool bLock);

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	bool GetGameMovementInputLock();

	UEmberLayerBase* GetLayer(FGameplayTag LayerTag) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|HUD")
	TSubclassOf<UUserWidget> PrimaryLayoutClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TMap<FGameplayTag, TObjectPtr<UEmberLayerBase>> EmberLayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|HUD")
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> InitWidgetClasses;

private:
	bool bIsGameLeftMouseInputLock{false};
	bool bIsGameMovementInputLock{false};
	//#if !UE_BUILD_SHIPPING
public:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void ToggleDebugLayer();

protected:
	TObjectPtr<ULayerDebugger> PrimaryDebugLayer;
	bool bDebugLayerVisible = false;
	//#endif
};
