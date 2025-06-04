#include "ProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Character/EmberCharacter.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "GameInstance/EffectManagerSubsystem.h"

AProjectileBase::AProjectileBase()
{
    PrimaryActorTick.bCanEverTick = false;
    
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootScene);
    
    SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    SphereComp->SetupAttachment(RootScene);
    // Blueprint에서 추가로 CollisionEnabled, CollisionProfile 설정해 줘야 함
    SphereComp->SetSphereRadius(20.f);
    SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // 혹은 QueryAndPhysics
    SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
    SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
    SphereComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    SphereComp->SetGenerateOverlapEvents(false); // Hit 이벤트만 원하면 false
    SphereComp->SetNotifyRigidBodyCollision(true); // Hit 발생 시 알리도록

    SphereComp->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(SphereComp);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed     = 2000.f;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->SetUpdatedComponent(SphereComp);
}

void AProjectileBase::BeginPlay()
{
    Super::BeginPlay();
    
    if (SphereComp && GetOwner())
    {
        SphereComp->IgnoreActorWhenMoving(GetOwner(), true);
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
        UGameInstance* GameInstance = GetWorld()->GetGameInstance();
        if (!GameInstance)
        {
            return;
        }
        
        UEffectManagerSubsystem* EffectManager = GameInstance->GetSubsystem<UEffectManagerSubsystem>();
        if (!EffectManager)
        {
            return;
        }
        
        if (Cast<ACharacter>(OtherActor))
        {
            if (AEmberCharacter* Player = Cast<AEmberCharacter>(GetOwner()))
            {
                EffectManager->PlayHitEffectAtLocation(Player->GetOverlayHitEffect(),Hit.Location,
                    FRotator::ZeroRotator,FVector(1.f, 1.f, 1.f),true);

                if (UAbilitySystemComponent* TargetAbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
                {
                    FGameplayEffectContextHandle EffectContextHandle = TargetAbilityComponent->MakeEffectContext();
                    EffectContextHandle.AddSourceObject(this);
                    FGameplayEffectSpecHandle EffectSpecHandle = TargetAbilityComponent->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContextHandle);
                    if (EffectSpecHandle.IsValid())
                    {
                        TargetAbilityComponent->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
                    }
                }
            }    
        }
    }
}
