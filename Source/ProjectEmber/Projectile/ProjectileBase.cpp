#include "ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Character/EmberCharacter.h"
#include "GameInstance/EffectManagerSubsystem.h"

AProjectileBase::AProjectileBase()
{
    PrimaryActorTick.bCanEverTick = false;
    
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);
    
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(RootScene);
    MeshComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    MeshComp->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
    
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed     = 2000.f;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->bRotationFollowsVelocity = false;

    /* 실제 충돌 체크할 컴포넌트 세팅 */
    ProjectileMovement->SetUpdatedComponent(MeshComp);
}

void AProjectileBase::BeginPlay()
{
    Super::BeginPlay();
    
    if (MeshComp && GetOwner())
    {
        MeshComp->IgnoreActorWhenMoving(GetOwner(), true);
    }

    SetLifeSpan(LifeSpanSeconds);
}

void AProjectileBase::FireInDirection(const FVector& ShootDirection)
{
    SetActorRotation(ShootDirection.Rotation());
    
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
    }
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
                            const FHitResult& Hit)
{
    if (OtherActor == GetOwner())
    {
        return;
    }
    
    if (OtherActor && OtherActor != this)
    {
        UGameplayStatics::ApplyPointDamage(
            OtherActor,
            Damage,
            GetActorLocation(),
            Hit,
            GetInstigatorController(),
            this,
            nullptr
        );

        if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
        {
            if (UEffectManagerSubsystem* EffectManager = GameInstance->GetSubsystem<UEffectManagerSubsystem>())
            {
                if (AEmberCharacter* Player = Cast<AEmberCharacter>(GetOwner()))
                {
                    EffectManager->PlayHitEffectAtLocation(Player->GetOverlayHitEffect(),Hit.Location,
                        FRotator::ZeroRotator,FVector(1.f, 1.f, 1.f),true);    
                }
            }    
        }
    }
    
    //Destroy();
}
