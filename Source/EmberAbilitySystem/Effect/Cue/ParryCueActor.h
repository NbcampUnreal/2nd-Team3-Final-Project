#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "Components/PostProcessComponent.h"
#include "ParryCueActor.generated.h"

class UNiagaraSystem;
class UPostProcessComponent;

UCLASS()
class EMBERABILITYSYSTEM_API AParryCueActor : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:
	AParryCueActor();

	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
	virtual bool OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cue|VFX")
	TObjectPtr<UParticleSystem> CueEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cue|VFX")
	TObjectPtr<UNiagaraSystem> CueNiagara;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cue|SFX")
	TObjectPtr<USoundBase> CueSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cue|Camera")
	TSubclassOf<UCameraShakeBase> CueCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cue|Camera")
	float CameraShakeScale = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cue|PostProcess")
	TObjectPtr<UMaterialInterface> PostProcessMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cue|PostProcess")
	TObjectPtr<UMaterialInterface> PostProcessSpeedLineMaterial;
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Cue|PostProcess")
	TObjectPtr<UPostProcessComponent> PostProcessComp;
};
