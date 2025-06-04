#include "EffectManagerSubsystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "EmberLog/EmberLog.h"
#include "Engine/World.h"

void UEffectManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UEffectManagerSubsystem::PlayHitEffectAtLocation(UNiagaraSystem* EffectAsset,const FVector& SpawnLocation,
    const FRotator& SpawnRotation,const FVector& Scale,bool bAutoDestroy)
{
    if (!EffectAsset)
    {
        EMBER_LOG(LogEmber, Warning, TEXT("EffectAsset is null."));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        EMBER_LOG(LogEmber, Warning, TEXT("World is null."));
        return;
    }

    SpawnNiagaraAtLocation(EffectAsset, SpawnLocation, SpawnRotation, Scale, bAutoDestroy);
}

void UEffectManagerSubsystem::PlayEffectOnActor(UNiagaraSystem* EffectAsset,AActor* TargetActor,
    const FVector& RelativeOffset,const FRotator& SpawnRotation,const FVector& Scale,bool bAutoDestroy)
{
    if (!EffectAsset)
    {
        EMBER_LOG(LogEmber, Warning, TEXT("EffectAsset is null."));
        return;
    }

    if (!TargetActor)
    {
        EMBER_LOG(LogEmber, Warning, TEXT("TargetActor is null."));
        return;
    }

    USceneComponent* AttachTo = TargetActor->GetRootComponent();
    if (!AttachTo)
    {
        EMBER_LOG(LogEmber, Warning, TEXT("TargetActor->RootComponent is null."));
        return;
    }

    SpawnNiagaraAttached(EffectAsset, AttachTo, RelativeOffset, SpawnRotation, Scale, bAutoDestroy);
}

void UEffectManagerSubsystem::SpawnNiagaraAtLocation(UNiagaraSystem* EffectAsset,const FVector& Location,
    const FRotator& Rotation,const FVector& Scale,bool bAutoDestroy)
{
    const UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    
    UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        World,
        EffectAsset,
        Location,
        Rotation,
        Scale,
        true,                   // AutoActivate
        bAutoDestroy,           // AutoDestroy (true면 재생 후 자동으로 풀에 반환)
        ENCPoolMethod::AutoRelease
    );
}

void UEffectManagerSubsystem::SpawnNiagaraAttached(UNiagaraSystem* EffectAsset,USceneComponent* AttachTo,
    const FVector& RelativeLocation,const FRotator& Rotation,const FVector& Scale,bool bAutoDestroy)
{
    UWorld* World = GetWorld();
    if (!World || !AttachTo)
    {
        return;
    }
    
    UNiagaraFunctionLibrary::SpawnSystemAttached(
        EffectAsset,
        AttachTo,
        NAME_None,
        RelativeLocation,
        Rotation,
        Scale,
        EAttachLocation::KeepRelativeOffset,
        true,
        ENCPoolMethod::AutoRelease
    );
}
