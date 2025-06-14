#include "ParryCueActor.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/PostProcessComponent.h"
#include "Camera/CameraShakeBase.h"
#include "EmberLog/EmberLog.h"

AParryCueActor::AParryCueActor()
{
    PostProcessComp = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComp"));
    PostProcessComp->SetupAttachment(GetRootComponent());
    PostProcessComp->bEnabled = false;
    PostProcessComp->bUnbound = true;
}

bool AParryCueActor::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    if (CueEffect && MyTarget)
    {
        UGameplayStatics::SpawnEmitterAttached(CueEffect, MyTarget->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale);
    }
    if (CueNiagara && MyTarget)
    {
        //UNiagaraFunctionLibrary::SpawnSystemAttached(CueNiagara, MyTarget->GetRootComponent(), NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTargetIncludingScale);
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),CueNiagara, MyTarget->GetActorLocation(),FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true);
    }
    if (CueSound && MyTarget)
    {
        UGameplayStatics::PlaySoundAtLocation(MyTarget, CueSound, MyTarget->GetActorLocation());
    }

    if (CueCameraShake && MyTarget)
    {
        if (APawn* Pawn = Cast<APawn>(MyTarget))
        {
            if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
            {
                PC->ClientStartCameraShake(CueCameraShake, CameraShakeScale);
            }
        }
    }

    if (PostProcessMaterial && PostProcessComp)
    {
        // Add material to blendables with full weight
        PostProcessComp->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.0f, PostProcessMaterial));
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
