#include "ParryCueActor.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/PostProcessComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "EmberLog/EmberLog.h"

AParryCueActor::AParryCueActor()
{
    if (!GetRootComponent())
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    }
    
    PostProcessComp = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComp"));
    PostProcessComp->SetupAttachment(GetRootComponent());
    PostProcessComp->bEnabled = false;
    PostProcessComp->bUnbound = true;
}

bool AParryCueActor::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    if (!MyTarget)
    {
        EMBER_LOG(LogEmber, Error, TEXT("ParryCueActor: MyTarget is null"));
        return false;
    }
    
    if (CueEffect)
    {
        UGameplayStatics::SpawnEmitterAttached(CueEffect, MyTarget->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale);
    }
    if (CueNiagara)
    {
        //UNiagaraFunctionLibrary::SpawnSystemAttached(CueNiagara, MyTarget->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),CueNiagara, MyTarget->GetActorLocation(),FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true);
    }
    if (CueSound)
    {
        UGameplayStatics::PlaySoundAtLocation(MyTarget, CueSound, MyTarget->GetActorLocation());
    }

    if (CueCameraShake)
    {
        if (APawn* Pawn = Cast<APawn>(MyTarget))
        {
            if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
            {
                PC->ClientStartCameraShake(CueCameraShake, CameraShakeScale);
            }
        }
    }

    if (PostProcessMaterial && PostProcessSpeedLineMaterial && PostProcessComp)
    {
        // Add material to blendables with full weight
        PostProcessComp->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, PostProcessMaterial));
        PostProcessComp->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, PostProcessSpeedLineMaterial));
        PostProcessComp->bEnabled = true;
    }

    return true;
}

bool AParryCueActor::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    if (PostProcessComp)
    {
        EMBER_LOG(LogEmber, Warning, TEXT("PostProcessComp is valid, disabling and clearing blendables."));
        PostProcessComp->bEnabled = false;
        PostProcessComp->Settings.WeightedBlendables.Array.Empty(); 
    }
    return true;
}
