#include "AbilityInteractBox.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AAbilityInteractBox::AAbilityInteractBox()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	
	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);
	
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT(
		"/Script/Engine.StaticMesh'/Engine/EngineMeshes/Cube.Cube'"));
	if (BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.2f, 0.2f, 0.2f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AAbilityInteractBox::BeginPlay()
{
	Super::BeginPlay();
	
}

class UAbilitySystemComponent* AAbilityInteractBox::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAbilityInteractBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	InvokeGameplayCue(OtherActor);
	ApplyEffectToTarget(OtherActor);
	
	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(2.0f);
}

void AAbilityInteractBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
}

void AAbilityInteractBox::ApplyEffectToTarget(AActor* TargetActor)
{
	if (UAbilitySystemComponent* TargetAbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
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

void AAbilityInteractBox::InvokeGameplayCue(AActor* TargetActor)
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = TargetActor;
	Param.Location = GetActorLocation();

	AbilitySystemComponent->ExecuteGameplayCue(GameplayCueTag, Param);
}
