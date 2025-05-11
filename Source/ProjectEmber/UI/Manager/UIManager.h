#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIManager.generated.h"

class FSubsystemCollectionBase;
enum class ELayer : FPlatformTypes::uint8;

UCLASS()
class PROJECTEMBER_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void RegisterLayer(ELayer Layer, const TSubclassOf<class UEmberBaseWidget>& WidgetClass);

	void Open(ELayer Layer);
	void Close(ELayer Layer);
	void Toggle(ELayer Layer);

private:
	UEmberBaseWidget* GetOrCreate(ELayer Layer);

	TMap<ELayer, TSubclassOf<UEmberBaseWidget>> WidgetClasses;
	TMap<ELayer, TObjectPtr<UEmberBaseWidget>>  Widgets;
};
