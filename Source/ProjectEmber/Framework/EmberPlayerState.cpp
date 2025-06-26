#include "EmberPlayerState.h"
#include "AbilitySystemComponent.h"
#include "SkillManagerSubsystem.h"
#include "SkillManagerSubsystem.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Attribute/Player/EmberPlayerAttributeSet.h"
#include "Item/Ability/EmberItemAttributeSet.h"
#include "Quest/QuestSubsystem.h"

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

	/*auto* Proxy = UEMSAsyncLoad::LoadGameActors(GetWorld(), true, true);
	if (Proxy)
	{
		// 완료 시 호출될 함수 바인딩
		Proxy->Completed.AddUObject(this, &USkillManagerSubsystem::OnAllActorsLoaded);
	}*/
}

void AEmberPlayerState::ActorPreSave_Implementation()
{
	if (UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		QuestProgress = QuestSubsystem->GetQuestProgress();	
	}
}

void AEmberPlayerState::ActorLoaded_Implementation()
{
	if (UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
	{
		QuestSubsystem->LoadQuest(GetPlayerController(), QuestProgress);
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

