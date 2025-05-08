#include "Effect/Cue/GC_AttackHit.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"

UGC_AttackHit::UGC_AttackHit()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionRef(
	TEXT("/Script/Engine.ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'")
);
	
	if (ExplosionRef.Succeeded())
	{
		ParticleSystem = ExplosionRef.Object;
	}
}

bool UGC_AttackHit::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	if (const FHitResult* HitResult = Parameters.EffectContext.GetHitResult())
	{
		UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, HitResult->ImpactPoint, FRotator::ZeroRotator, true);
	}
	
	return Super::OnExecute_Implementation(Target, Parameters);
}
