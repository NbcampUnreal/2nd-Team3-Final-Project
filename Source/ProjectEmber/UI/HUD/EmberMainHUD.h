#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayTagContainer.h"
#include "AI_NPC/Widget/PlayerQuestWidget.h"
#include "Interface/EmberHUDInterface.h"
#include "EmberMainHUD.generated.h"

class ULayerDebugger;
class UEmberLayerBase;

UCLASS(Blueprintable, BlueprintType)
class PROJECTEMBER_API AEmberMainHUD : public AHUD, public IEmberHUDInterface
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	virtual bool RegisterLayer(const FGameplayTag& LayerTag, UUserWidget* Layer) override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	virtual UUserWidget* PushContentToLayer(const FGameplayTag& LayerTag, const TSubclassOf<UUserWidget>& WidgetClass) override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	virtual void PopContentToLayer(const FGameplayTag& LayerTag) override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	virtual void ClearToLayer(const FGameplayTag& LayerTag) override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	virtual bool GetGameLeftMouseInputLock() const override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	virtual void SetGameLeftMouseInputLock(bool bLock) override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	virtual bool GetGameMovementInputLock() const override;

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	virtual void SetGameMovementInputLock(bool bLock) override;
	
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void PushInitialWidget();

	UEmberLayerBase* GetLayer(const FGameplayTag& LayerTag) const;
	
	UFUNCTION(BlueprintCallable)
	UPlayerQuestWidget* GetQuestLogWidget() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UPlayerQuestWidget> PlayerQuestWidgetClass;

	UPROPERTY()
	UPlayerQuestWidget* PlayerQuestWidgetInstance;



protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|HUD")
	TSubclassOf<UUserWidget> PrimaryLayoutClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TMap<FGameplayTag, TObjectPtr<UEmberLayerBase>> EmberLayers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|HUD")
	TMap<FGameplayTag, TSubclassOf<UUserWidget>> InitWidgetClasses;

private:
	bool bIsGameLeftMouseInputLock{ false };
	bool bIsGameMovementInputLock{ false };

public:
//#if !UE_BUILD_SHIPPING
	//UFUNCTION(BlueprintCallable, Category = "UI|HUD")


protected:
	TObjectPtr<ULayerDebugger> PrimaryDebugLayer;
	bool bDebugLayerVisible = false;
//#endif
};
