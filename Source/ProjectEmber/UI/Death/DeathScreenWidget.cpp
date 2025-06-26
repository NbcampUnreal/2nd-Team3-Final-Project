#include "DeathScreenWidget.h"
#include "ActiveGameplayEffectHandle.h"
#include "EngineUtils.h"
#include "AbilitySystemComponent.h"
#include "Character/RespawnTarget/RespawnTarget.h"
#include "FunctionLibrary/UIFunctionLibrary.h"

void UDeathScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RespawnButton)
	{
		RespawnButton->OnClicked.AddDynamic(this, &UDeathScreenWidget::OnRespawnClicked);
	}
}

void UDeathScreenWidget::OnRespawnClicked()
{
	const ARespawnTarget* NearestTarget = nullptr;
	float MinDistSq = FLT_MAX;
	const FVector OwnerLocation = OwningCharacter->GetActorLocation();

	for (TActorIterator<ARespawnTarget> It(GetWorld()); It; ++It)
	{
		const ARespawnTarget* Target = *It;
		if (!Target)
		{
			continue;
		}
		
		const float DistSq = FVector::DistSquared(OwnerLocation, Target->GetActorLocation());
		if (DistSq < MinDistSq)
		{
			MinDistSq = DistSq;
			NearestTarget = Target;
		}
	}
	
	if (NearestTarget)
	{
		OwningCharacter->SetActorLocation(NearestTarget->GetActorLocation());
		if (UAbilitySystemComponent* Asc = OwningCharacter->GetAbilitySystemComponent())
		{
			const FGameplayEffectQuery AllQuery = FGameplayEffectQuery::MakeQuery_MatchAnyEffectTags(FGameplayTagContainer{});
			TArray<FActiveGameplayEffectHandle> Handles = Asc->GetActiveEffects(AllQuery);
			
			for (const auto& Handle : Handles)
			{
				Asc->RemoveActiveGameplayEffect(Handle);
			}

			static const TSubclassOf<UGameplayEffect> InitEffectClass =	LoadClass<UGameplayEffect>(nullptr,TEXT("/Script/Engine.Blueprint'/Game/_Blueprints/AbilitySystem/Effect/Init/BPGE_InitAttributes.BPGE_InitAttributes_C'"));
			if (InitEffectClass)
			{
				Asc->ApplyGameplayEffectToSelf(InitEffectClass.GetDefaultObject(), 1.0f, Asc->MakeEffectContext());
			}
		}
		
		// 까먹지 말고 이펙트 넣자
		APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController());
		UUIFunctionLibrary::PopContentToLayer(PlayerController, FGameplayTag::RequestGameplayTag("UI.Layer.Modal"));
		UUIFunctionLibrary::FocusGame(PlayerController);
	}
}

void UDeathScreenWidget::SetOwner(APawn* NewOwner)
{
	OwningCharacter = Cast<AEmberCharacter>(NewOwner);
}
