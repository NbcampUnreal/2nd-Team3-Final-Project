#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UProjectileMovementComponent;

UCLASS()
class PROJECTEMBER_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AProjectileBase();
	
protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable, Category="Projectile")
	void FireInDirection(const FVector& ShootDirection);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	float Damage = 10.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	float LifeSpanSeconds = 5.f;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	USceneComponent* RootScene;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UProjectileMovementComponent* ProjectileMovement;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,FVector NormalImpulse,
		const FHitResult& Hit);
};
