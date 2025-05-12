#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManager.generated.h"

class UEmberBaseWidget;
class UUIManagerHelper;
class FSubsystemCollectionBase;
enum class ELayer : FPlatformTypes::uint8;

UCLASS(Config = Game, DefaultConfig)
class PROJECTEMBER_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void Open(ELayer Layer);
	void Close(ELayer Layer);
	void Toggle(ELayer Layer);

private:
	UEmberBaseWidget* GetOrCreate(ELayer Layer);

	UPROPERTY(Config)
	TSubclassOf<UUIManagerHelper> WidgetClassesHelperClass;
	
	UPROPERTY()
	UUIManagerHelper* WidgetClassesHelper;
	
	TMap<ELayer, TObjectPtr<UEmberBaseWidget>>  Widgets;
};
