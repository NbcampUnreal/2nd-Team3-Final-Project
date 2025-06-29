#include "EmberPlayerState.h"
#include "AbilitySystemComponent.h"
#include "SkillManagerSubsystem.h"
#include "SkillManagerSubsystem.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Attribute/Player/EmberPlayerAttributeSet.h"
#include "Character/EmberCharacter.h"
#include "Item/UserItemManger.h"
#include "EmberLog/EmberLog.h"
#include "Item/Ability/EmberItemAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestSubsystem.h"
#include "Attribute/SaveData/AttributeSaveData.h"


AEmberPlayerState::AEmberPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AttributeSet = CreateDefaultSubobject<UEmberCharacterAttributeSet>(TEXT("AttributeSet"));
	PlayerAttributeSet = CreateDefaultSubobject<UEmberPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
	ItemAttributeSet = CreateDefaultSubobject<UEmberItemAttributeSet>(TEXT("ItemAttributeSet"));
	AbilitySystemComponent->AddAttributeSetSubobject<UEmberCharacterAttributeSet>(AttributeSet);
	AbilitySystemComponent->AddAttributeSetSubobject<UEmberItemAttributeSet>(ItemAttributeSet);

	AbilitySystemComponent->SetIsReplicated(true);
	
}

void AEmberPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	if (AttributeSet)
	{
		AttributeSet->Initialize(AbilitySystemComponent);
		PlayerAttributeSet->Initialize(AbilitySystemComponent);
	}

	AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AEmberPlayerState::HandleAbilityFailed);
}

void AEmberPlayerState::ActorPreSave_Implementation()
{
	if (AttributeSet)
	{
		AttributeSet->FillSaveData(CharacterAttributeSaveData);		
	}
	
	if (UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		QuestProgress = QuestSubsystem->GetQuestProgress();
	}

	if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (EmberCharacter->GetItemManager())
		{
			EmberCharacter->GetItemManager()->SaveInventoryItem(InventoryProgress);
			EmberCharacter->GetItemManager()->SaveQuickSlotItem(QuickSlotProgress);
			EmberCharacter->GetItemManager()->SaveEquipmentItem(EquipmentProgress);
		}
	}
}

void AEmberPlayerState::ActorLoaded_Implementation()
{
	if (AttributeSet)
	{
		AttributeSet->LoadSaveData(CharacterAttributeSaveData);		
	}
	
	if (UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		QuestSubsystem->LoadQuest(GetPlayerController(), QuestProgress);
	}
	
	if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
	{
		if (EmberCharacter->GetItemManager())
		{
			EmberCharacter->GetItemManager()->LoadInventoryItem(InventoryProgress);
			EmberCharacter->GetItemManager()->LoadQuickSlotItem(QuickSlotProgress);
			EmberCharacter->GetItemManager()->LoadEquipmentItem(EquipmentProgress);
		}
	}
}

void AEmberPlayerState::HandleAbilityFailed(const UGameplayAbility* FailedAbility,
	const FGameplayTagContainer& FailureTags)
{
	if (FailedAbility)
	{
		UE_LOG(LogEmber, Warning,
			TEXT("[Ability Failed] %s 실패 이유: %s"),
			*FailedAbility->GetName(),
			*FailureTags.ToStringSimple());
	}
	else
	{
		UE_LOG(LogEmber, Warning,
			TEXT("[Ability Failed] 알 수 없는 어빌리티 실패. 이유: %s"),
			*FailureTags.ToStringSimple());
	}
}


void AEmberPlayerState::GameMenuWidgetLoaded()
{
	if (UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		QuestSubsystem->LoadQuest(GetPlayerController(), QuestProgress);
	}
}

class UAbilitySystemComponent* AEmberPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

