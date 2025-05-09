#include "Character/EmberComponents/InteractionComponent.h"
#include "Character/EmberCharacter.h"
#include "Interactables/BaseInteractableActor.h"
#include "Interactables/BasePickupActor.h"

#include "Components/BoxComponent.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/CharacterMovementComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	GatherTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("GatherTrigger"));
	GatherTrigger->InitBoxExtent(FVector(100.f));
	GatherTrigger->SetCollisionProfileName(TEXT("Trigger"));
	
	PickupTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupTrigger"));
	PickupTrigger->InitBoxExtent(FVector(100.f));
	PickupTrigger->SetCollisionProfileName(TEXT("Trigger"));
}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AEmberCharacter* Character = Cast<AEmberCharacter>(GetOwner()))
	{
		if (Character->InteractMontage)
		{
			InteractMontage = Character->InteractMontage;
		}
	}
}

void UInteractionComponent::OnRegister()
{
	Super::OnRegister();

	if (GetOwner()->GetRootComponent())
	{
		GatherTrigger->SetupAttachment(GetOwner()->GetRootComponent());
		PickupTrigger->SetupAttachment(GetOwner()->GetRootComponent());
	}

	FVector GatherTriggerLocation(50.0f, 0.0f, 0.0f);
	GatherTrigger->SetRelativeLocation(GatherTriggerLocation);
	GatherTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	GatherTrigger->SetGenerateOverlapEvents(true);
	GatherTrigger->RegisterComponent();

	PickupTrigger->SetRelativeLocation(FVector::ZeroVector);
	PickupTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	PickupTrigger->SetGenerateOverlapEvents(true);
	PickupTrigger->RegisterComponent();

	GatherTrigger->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnGatherOverlapBegin);
	GatherTrigger->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnGatherOverlapEnd);

	PickupTrigger->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnPickupOverlapBegin);
	PickupTrigger->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnPickupOverlapEnd);
}

void UInteractionComponent::OnUnregister()
{
	Super::OnUnregister();

	GatherTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractionComponent::OnGatherOverlapBegin);
	GatherTrigger->OnComponentEndOverlap.RemoveDynamic(this, &UInteractionComponent::OnGatherOverlapEnd);
	
	PickupTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractionComponent::OnPickupOverlapBegin);
	PickupTrigger->OnComponentEndOverlap.RemoveDynamic(this, &UInteractionComponent::OnPickupOverlapEnd);
}

void UInteractionComponent::OnGatherOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseInteractableActor* Actor = Cast<ABaseInteractableActor>(OtherActor))
	{
		EMBER_LOG(LogTemp, Warning, TEXT("Gather Event Activate"));
		SetCurrentInteractable(Actor);
	}
}

void UInteractionComponent::OnGatherOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseInteractableActor* Actor = Cast<ABaseInteractableActor>(OtherActor))
	{
		SetCurrentInteractable(nullptr);
	}
}

void UInteractionComponent::OnPickupOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseInteractableActor* Actor = Cast<ABaseInteractableActor>(OtherActor))
	{
		EMBER_LOG(LogTemp, Warning, TEXT("Pickup Event Activate"));
		if (ABasePickupActor* DetailActor = Cast<ABasePickupActor>(OtherActor))
		{
			SetCurrentInteractable(DetailActor);
		}
	}
}

void UInteractionComponent::OnPickupOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseInteractableActor* Actor = Cast<ABaseInteractableActor>(OtherActor))
	{
		if (ABasePickupActor* DetailActor = Cast<ABasePickupActor>(OtherActor))
		{
			EMBER_LOG(LogTemp, Warning, TEXT("Pickup Event Deactivate"));
			SetCurrentInteractable(nullptr);
		}
	}
}

void UInteractionComponent::SetCurrentInteractable(UObject* NewInteractable)
{
	CurrentInteractable = NewInteractable;
}

void UInteractionComponent::Interact()
{
	if (CurrentInteractable)
	{
		GatherTime = IInteractable::Execute_GetGatherTime(CurrentInteractable.GetObject());

		EMBER_LOG(LogTemp, Warning, TEXT("Interact,  GatherTime: %f"), GatherTime);
		if (GatherTime <= 0.0f)
		{
			IInteractable::Execute_Interact(CurrentInteractable.GetObject(), GetOwner());
			PickupTrigger->UpdateOverlaps();
		}
		else
		{
			StartGather();
		}
	}
}

void UInteractionComponent::StartGather()
{
	PlayInteractMontage();
	
	/*if (!GatherWidget)
	{
		GatherWidget = CreateWidget<UWBP_GatherBar>(GetWorld(), UWBP_GatherBar::StaticClass());
		if (GatherWidget)
		{
			GatherWidget->AddToViewport();
		}
	}*/

	GatherElapsed = 0.f;
	
	// 반복 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(
		GatherTimerHandle,
		this,
		&UInteractionComponent::GatherTick,
		GatherTickInterval,
		true
	);
}

void UInteractionComponent::StopGather()
{
	// 중단 시, 타이머·게이지 초기화
	GetWorld()->GetTimerManager().ClearTimer(GatherTimerHandle);
	if (GatherTime > 0.f)
	{
		StopInteractMontage();
	}
	else
	{
		GatherElapsed = 0.f;
	}
	/*if (GatherWidget)
	{
		GatherWidget->RemoveFromParent();
		GatherWidget = nullptr;
	}*/

	PickupTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PickupTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupTrigger->SetGenerateOverlapEvents(false);
	PickupTrigger->SetGenerateOverlapEvents(true);
	PickupTrigger->UpdateOverlaps();
}

void UInteractionComponent::GatherTick()
{
	GatherElapsed += GatherTickInterval;
	float Percent = FMath::Clamp(GatherElapsed / GatherTime, 0.f, 1.f);

	/*if (GatherWidget)
	{
		GatherWidget->SetPercent(Percent);
	}*/

	if (Percent >= 1.f)
	{
		// 타이머 정리
		GetWorld()->GetTimerManager().ClearTimer(GatherTimerHandle);

		// 인터페이스 호출: 채집 완료
		IInteractable::Execute_Interact(CurrentInteractable.GetObject(), GetOwner());

		// 위젯 닫기
		/*if (GatherWidget)
		{
			GatherWidget->RemoveFromParent();
			GatherWidget = nullptr;
		}*/
		
		StopInteractMontage();
	}
}

void UInteractionComponent::PlayInteractMontage()
{
	if (AEmberCharacter* Char = Cast<AEmberCharacter>(GetOwner()))
	{
		bLoopMontage = true;
		
		if (Char->GetCharacterMovement())
		{
			Char->GetCharacterMovement()->DisableMovement();
		}

		if (InteractMontage && Char->GetMesh())
		{
			EMBER_LOG(LogTemp, Warning, TEXT("AnumMontage Start"));
			if (UAnimInstance* AnimInst = Char->GetMesh()->GetAnimInstance())
			{
				CachedAnimInstance = AnimInst;
				
				CachedAnimInstance->OnMontageEnded.AddDynamic(
					this, &UInteractionComponent::OnMontageEnded
				);
				
				float Duration = AnimInst->Montage_Play(InteractMontage, 1.0f);
				EMBER_LOG(LogTemp, Warning, TEXT("Montage_Play returned duration %f"), Duration);
				EMBER_LOG(LogTemp, Warning, TEXT("Binding OnMontageEnded to %s"), *InteractMontage->GetName());
			}
		}
		else
		{
			EMBER_LOG(LogTemp, Warning, TEXT("AnumMontage is NULL"));
		}
	}
}

void UInteractionComponent::StopInteractMontage()
{
	bLoopMontage = false;
	
	if (CachedAnimInstance && InteractMontage)
	{
		FOnMontageEnded EndDel;
		CachedAnimInstance->Montage_Stop(0.2f, InteractMontage);
		CachedAnimInstance->Montage_SetEndDelegate(EndDel, InteractMontage);
		CachedAnimInstance = nullptr;
	}

	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (Char->GetCharacterMovement())
		{
			EMBER_LOG(LogTemp, Warning, TEXT("OnMontageEnded fired!"));
			Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
}

void UInteractionComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	EMBER_LOG(LogTemp, Warning,
		TEXT(">> OnMontageEnded called: Montage=%s, Interrupted=%s"),
		Montage ? *Montage->GetName() : TEXT("null"),
		bInterrupted ? TEXT("true") : TEXT("false")
	);
	
	if (bLoopMontage && Montage == InteractMontage && CachedAnimInstance)
	{
		CachedAnimInstance->OnMontageEnded.RemoveDynamic(
			this, &UInteractionComponent::OnMontageEnded
		);
		
		// ② 재생 (PlayRate은 필요 시 조정)
		CachedAnimInstance->Montage_Play(InteractMontage, 1.0f);
	}
	else if (!bLoopMontage)
	{
		// 반복 해제 시 처리 (이동 재개 등)
		if (AEmberCharacter* Char = Cast<AEmberCharacter>(GetOwner()))
		{
			if (Char->GetCharacterMovement())
			{
				Char->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
		}
		CachedAnimInstance = nullptr;
	}
}
