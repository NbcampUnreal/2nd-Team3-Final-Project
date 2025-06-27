#include "EmberPlayerState.h"
#include "AbilitySystemComponent.h"
#include "SkillManagerSubsystem.h"
#include "SkillManagerSubsystem.h"
#include "Attribute/Character/EmberCharacterAttributeSet.h"
#include "Attribute/Player/EmberPlayerAttributeSet.h"
#include "EmberLog/EmberLog.h"
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

	AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AEmberPlayerState::HandleAbilityFailed);
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

