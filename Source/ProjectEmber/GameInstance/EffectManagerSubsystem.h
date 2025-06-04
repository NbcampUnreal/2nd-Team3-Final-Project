#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NiagaraSystem.h"
#include "EffectManagerSubsystem.generated.h"

UCLASS()
class PROJECTEMBER_API UEffectManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	/**
	 * 월드 좌표에 Hit 이펙트 스폰
	 * @param EffectAsset    재생할 Niagara System 에셋
	 * @param SpawnLocation  월드 좌표
	 * @param SpawnRotation  (선택) 회전값, 기본은 ZeroRotator
	 * @param Scale          (선택) 스케일, 기본은 (1,1,1)
	 * @param bAutoDestroy   (선택) 재생 후 자동 풀 반환 여부, 기본 true
	 */
	UFUNCTION(BlueprintCallable, Category = "Effects|Manager")
	void PlayHitEffectAtLocation(UNiagaraSystem* EffectAsset, const FVector& SpawnLocation, const FRotator& SpawnRotation = FRotator::ZeroRotator, const FVector& Scale = FVector(1.f, 1.f, 1.f), bool bAutoDestroy = true);

	/**
	 * Actor에 붙여서 이펙트 스폰
	 * @param EffectAsset    재생할 Niagara System 에셋
	 * @param TargetActor    붙일 대상 액터
	 * @param RelativeOffset (선택) 액터 로컬 오프셋, 기본은 ZeroVector
	 * @param SpawnRotation  (선택) 회전값, 기본은 ZeroRotator
	 * @param Scale          (선택) 스케일, 기본은 (1,1,1)
	 * @param bAutoDestroy   (선택) 재생 후 자동 풀 반환 여부, 기본 true
	 */
	UFUNCTION(BlueprintCallable, Category = "Effects|Manager")
	void PlayEffectOnActor(UNiagaraSystem* EffectAsset, AActor* TargetActor, const FVector& RelativeOffset = FVector::ZeroVector, const FRotator& SpawnRotation = FRotator::ZeroRotator, const FVector& Scale = FVector(1.f, 1.f, 1.f), bool bAutoDestroy = true);
	
private:
	void SpawnNiagaraAtLocation(UNiagaraSystem* EffectAsset, const FVector& Location, const FRotator& Rotation, const FVector& Scale, bool bAutoDestroy);
	void SpawnNiagaraAttached(UNiagaraSystem* EffectAsset, USceneComponent* AttachTo, const FVector& RelativeLocation, const FRotator& Rotation, const FVector& Scale, bool bAutoDestroy);
	
};
