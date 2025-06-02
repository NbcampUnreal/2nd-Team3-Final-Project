#include "Character/EmberComponents/InteractionComponent.h"
#include "Character/EmberCharacter.h"
#include "Interactables/BaseInteractableActor.h"
#include "Interactables/BasePickupActor.h"
#include "AI_NPC/DialogueComponent.h"
#include "Components/BoxComponent.h"
#include "EmberLog/EmberLog.h"
#include "GameFramework/CharacterMovementComponent.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	GatherTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("GatherTrigger"));
	GatherTrigger->InitBoxExtent(FVector(100.f));
	GatherTrigger->SetCollisionProfileName(TEXT("Trigger"));
	
	OnInteractOverTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("OnInteractOverTrigger"));
	OnInteractOverTrigger->InitBoxExtent(FVector(100.f));
	OnInteractOverTrigger->SetCollisionProfileName(TEXT("Trigger"));
}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInteractionComponent::OnRegister()
{
	Super::OnRegister();

	if (GetOwner()->GetRootComponent())
	{
		GatherTrigger->SetupAttachment(GetOwner()->GetRootComponent());
		OnInteractOverTrigger->SetupAttachment(GetOwner()->GetRootComponent());
	}

	FVector GatherTriggerLocation(50.0f, 0.0f, 0.0f);
	GatherTrigger->SetRelativeLocation(GatherTriggerLocation);
	GatherTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	GatherTrigger->SetGenerateOverlapEvents(true);
	GatherTrigger->RegisterComponent();

	OnInteractOverTrigger->SetupAttachment(GetOwner()->GetRootComponent());
	OnInteractOverTrigger->SetRelativeLocation(FVector(100.f, 0.f, 0.f));
	OnInteractOverTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
	OnInteractOverTrigger->SetGenerateOverlapEvents(true);
	OnInteractOverTrigger->RegisterComponent();

	GatherTrigger->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnGatherOverlapBegin);
	GatherTrigger->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnGatherOverlapEnd);

	OnInteractOverTrigger->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnInteractOverlapBegin);
	OnInteractOverTrigger->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnInteractOverlapEnd);

}

void UInteractionComponent::OnUnregister()
{
	Super::OnUnregister();

	GatherTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractionComponent::OnGatherOverlapBegin);
	GatherTrigger->OnComponentEndOverlap.RemoveDynamic(this, &UInteractionComponent::OnGatherOverlapEnd);
	
	OnInteractOverTrigger->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractionComponent::OnInteractOverlapBegin);
	OnInteractOverTrigger->OnComponentEndOverlap.RemoveDynamic(this, &UInteractionComponent::OnInteractOverlapEnd);


}

void UInteractionComponent::OnGatherOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseInteractableActor* Actor = Cast<ABaseInteractableActor>(OtherActor))
	{
		if (!Cast<ABasePickupActor>(Actor))
		{
			EMBER_LOG(LogTemp, Warning, TEXT("Gather Event Activate"));
			SetCurrentInteractable(Actor);
		}
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

void UInteractionComponent::OnInteractOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner()) return;

	if (ABasePickupActor* PickupActor = Cast<ABasePickupActor>(OtherActor))
	{
		EMBER_LOG(LogTemp, Warning, TEXT("[OverlapBegin] PickupActor detected"));
		SetCurrentInteractable(PickupActor);
		return;
	}
	for (UActorComponent* Comp : OtherActor->GetComponents())
	{
		if (Comp->Implements<UInteractable>())
		{
			EMBER_LOG(LogTemp, Warning, TEXT("[OverlapBegin] Talk component detected"));
			SetCurrentInteractable(Comp);
			return;
		}
	}
}
void UInteractionComponent::OnInteractOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;
	if (Cast<ABasePickupActor>(OtherActor))
	{
		EMBER_LOG(LogTemp, Warning, TEXT("[OverlapEnd] PickupActor exited"));
		SetCurrentInteractable(nullptr);
		return;
	}
	for (UActorComponent* Comp : OtherActor->GetComponents())
	{
		if (Comp->Implements<UInteractable>())
		{
			EMBER_LOG(LogTemp, Warning, TEXT("[OverlapEnd] Talk component exited"));
			SetCurrentInteractable(nullptr);
			return;
		}
	}
}

void UInteractionComponent::TriggerAdvanceDialogue()
{
	UObject* RawObj = CurrentInteractable.GetObject();
	if (RawObj && RawObj->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		if (UDialogueComponent* Dialogue = Cast<UDialogueComponent>(CurrentInteractable.GetObject()))
		{
			Dialogue->AdvanceDialogue();
		}
	}
}
void UInteractionComponent::SetCurrentInteractable(UObject* NewInteractable)
{
	if (bIsLocked)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current Interactable is locked"));
		return;
	}

	if (NewInteractable && NewInteractable->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		CurrentInteractable.SetObject(NewInteractable);
		CurrentInteractable.SetInterface(Cast<IInteractable>(NewInteractable));
		UE_LOG(LogTemp, Warning, TEXT("SetCurrentInteractable: %s"), *NewInteractable->GetName());
	}
	else
	{
		if (UDialogueComponent* Dialogue = Cast<UDialogueComponent>(CurrentInteractable.GetObject()))
		{
			if (Dialogue->IsDialogueActive())
			{
				return;
			}
		}
		CurrentInteractable = nullptr;
	}
}

void UInteractionComponent::Interact()
{
	if (!CurrentInteractable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact called with no interactable."));
		return;
	}

	UObject* RawObj = CurrentInteractable.GetObject();

	if (!RawObj || !RawObj->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("Interact target does not implement IInteractable."));
		return;
	}

	if (UDialogueComponent* Dialogue = Cast<UDialogueComponent>(RawObj))
	{
		if (Dialogue->IsDialogueActive())
		{
			return;
		}
	}

	GatherTime = IInteractable::Execute_GetGatherTime(RawObj);

	if (GatherTime <= 0.0f)
	{
		IInteractable::Execute_Interact(RawObj, GetOwner());
		OnInteractOverTrigger->UpdateOverlaps();
	}
	else
	{
		StartGather();
	}
}

void UInteractionComponent::StartGather()
{
	bIsLocked = true;
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

	bIsLocked = false;
	SetCurrentInteractable(nullptr);
	
	OnInteractOverTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OnInteractOverTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	GatherTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GatherTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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
			}
		}
		else
		{
			EMBER_LOG(LogTemp, Warning, TEXT("AnimMontage is NULL"));
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
		
		CachedAnimInstance->Montage_Play(InteractMontage, 1.0f);
	}
	else if (!bLoopMontage)
	{
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
