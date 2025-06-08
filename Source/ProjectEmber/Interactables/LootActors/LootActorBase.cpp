#include "LootActorBase.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Ability/Base/BaseOverlayAbility.h"
#include "Character/EmberCharacter.h"
#include "EmberLog/EmberLog.h"
#include "GameInstance/GameplayEventSubsystem.h"
#include "Item/ItemSubsystem.h"
#include "Item/Core/ItemSystemLibrary.h"
#include "Item/Drop/EmberInteractableItemDropComponent.h"
#include "Kismet/GameplayStatics.h"

ALootActorBase::ALootActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ItemDropComponent = CreateDefaultSubobject<UEmberInteractableItemDropComponent>(TEXT("ItemDropComponent"));
	SetRootComponent(MeshComponent);
}

// Called when the game starts or when spawned
void ALootActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALootActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALootActorBase::StartInteractAbility(APawn* InstigatorPawn)
{
	if (InstigatorPawn && InteractAbilityClass)
	{
		if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(InstigatorPawn))
		{
			if (UAbilitySystemComponent* AbilitySystem = EmberCharacter->GetAbilitySystemComponent())
			{
				FGameplayAbilitySpec* AbilitySpec = AbilitySystem->FindAbilitySpecFromClass(InteractAbilityClass);
				if (AbilitySpec->IsActive())
				{
					EMBER_LOG(LogEmber, Warning, TEXT("Interact Ability is already active on %s"), *EmberCharacter->GetName());
				}
				else
				{
					MessageBusSubscribe();
					
					const FVector TargetLocation = MeshComponent->GetComponentLocation();
					const FVector CharacterLocation = EmberCharacter->GetActorLocation();
					const FVector Dir = (TargetLocation - CharacterLocation).GetSafeNormal();
					SetCharacterRotation(EmberCharacter, Dir.Rotation().Yaw);

					SetLootTypeToHoldTime(AbilitySpec->GetPrimaryInstance());
					
					// 오버레이 전환 체크 및 어빌리티 재생
					PreOverlayTag = EmberCharacter->GetOverlayMode(); 
					EmberCharacter->SetOverlayMode(InteractOverlayTag);
					AbilitySystem->TryActivateAbility(AbilitySpec->Handle);
					TargetAbilitySystemComponent = AbilitySystem;
				}
			}
		}
	}
}

void ALootActorBase::UpdateInteractAbility()
{
	if (TargetAbilitySystemComponent && TargetAbilitySystemComponent->TryActivateAbilityByClass(InteractAbilityClass))
	{
		AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(TargetAbilitySystemComponent->GetAvatarActor());
		const FVector TargetLocation = MeshComponent->GetComponentLocation();
		const FVector CharacterLocation = EmberCharacter->GetActorLocation();
		const FVector Dir = (TargetLocation - CharacterLocation).GetSafeNormal();
		SetCharacterRotation(EmberCharacter, Dir.Rotation().Yaw);
	}
}

void ALootActorBase::CancelInteractAbility()
{
	if (TargetAbilitySystemComponent)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = TargetAbilitySystemComponent->FindAbilitySpecFromClass(InteractAbilityClass))
		{
			bIsAbilityEnded = true;
			TargetAbilitySystemComponent->CancelAbilityHandle(AbilitySpec->Handle);
		}	
	}
}
void ALootActorBase::CompleteInteractAbility()
{
	bIsAbilityEnded = true;

	FGameplayEventData EventData;
	EventData.EventTag = FGameplayTag::RequestGameplayTag(FName("Quest.Gathering"));
	EventData.Instigator = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	EventData.Target = this;

	//  태그 기반으로 대상 오브젝트 정보 부여
	if (TargetItemTag.IsValid())
	{
		EventData.TargetTags.AddTag(TargetItemTag);
	}

	if (UGameplayEventSubsystem* EventSystem = UGameplayEventSubsystem::GetGameplayEvent(GetWorld()))
	{
		EventSystem->OnGameEvent.Broadcast(EventData.EventTag, EventData);
		UE_LOG(LogTemp, Warning, TEXT(" [LootActor] Gathering 이벤트 발생: %s / 아이템 태그: %s"),
			*EventData.EventTag.ToString(), *TargetItemTag.ToString());
	}


	ItemDropComponent->DropItem(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	// 3. 아이템 획득 등의 후속 처리 (선택)
	// AddItemToInventory(...);

}

void ALootActorBase::RefreshOverlayMode(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(InstigatorPawn))
		{
			EmberCharacter->SetOverlayMode(PreOverlayTag); 
		}
	}
}

void ALootActorBase::SetLootTypeToHoldTime(UGameplayAbility* GameplayAbility)
{
	if (const UBaseOverlayAbility* OverlayAbility = Cast<UBaseOverlayAbility>(GameplayAbility))
	{
		float MontageLength = OverlayAbility->GetDefaultMontage()->GetPlayLength();
		float MontageRateScale = OverlayAbility->GetDefaultMontage()->RateScale;

		/* 공식 : (몽타주전체길이 / 재생속도:공격속도) * (몇번휘두를건지 - 1) + (몽타주에서 내려찍는 시간 / 재생속도:공격속도) */
		if (LootAbilityType == ELootAbilityType::Harvest)
		{
			SetHoldTime((MontageLength / MontageRateScale) - 0.3f);	
		}
		else if (LootAbilityType == ELootAbilityType::Tree)
		{
			SetHoldTime((MontageLength / MontageRateScale) * (SwingCount - 1) + (0.6f / MontageRateScale));
		}
		else if (LootAbilityType == ELootAbilityType::Mineral)
		{
			SetHoldTime((MontageLength / MontageRateScale) * (SwingCount - 1) + (0.95f / MontageRateScale));
		}
	}
}

void ALootActorBase::SetCharacterRotation(AEmberCharacter* EmberCharacter, const float YawAngle)
{
	EmberCharacter->ForceRoationTest(YawAngle);
	EmberCharacter->ForceLastInputDirectionBlocked(true);
}

void ALootActorBase::MessageBusSubscribe()
{
	MessageDelegateHandle = FMessageDelegate::CreateUObject(this, &ThisClass::ReceiveMessage);
	UMessageBus::GetInstance()->Subscribe(TEXT("OverlayAbilityEnded"), MessageDelegateHandle);
}

void ALootActorBase::MessageBusUnsubscribe()
{
	UMessageBus::GetInstance()->Unsubscribe(TEXT("OverlayAbilityEnded"), MessageDelegateHandle);
}

void ALootActorBase::ReceiveMessage(const FName MessageType, UObject* Payload)
{
	if (MessageType == TEXT("OverlayAbilityEnded") && bIsAbilityEnded)
	{
		RefreshOverlayMode(Cast<APawn>(Payload));
		MessageBusUnsubscribe();
		bIsAbilityEnded = false;
	}
}

