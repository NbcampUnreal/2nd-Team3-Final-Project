#include "AnimNotify_PlaySFX.h"

#include "GameInstance/AudioDataSettings.h"
#include "GameInstance/AudioSubsystem.h"
#include "GameInstance/Structs/CharacterAudioDataStruct.h"
#include "GameInstance/Structs/UISFXAudioDataStruct.h"

void UAnimNotify_PlaySFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation,EventReference);

    if (!MeshComp)
    {
        return;
    }

    UWorld* World = MeshComp->GetWorld();
    if (!World)
    {
        return;
    }
    
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner)
    {
        return;
    }
    
    // 에디터프리뷰에서 확인하기 위해서 분기처리함
    const bool bIsGameWorld = (World->WorldType == EWorldType::Game || World->WorldType == EWorldType::PIE);

    // 인게임
    if (bIsGameWorld)
    {
        const UGameInstance* GameInstance = Owner->GetGameInstance();
        if (!GameInstance)
        {
            return;
        }

        UAudioSubsystem* AudioManager = GameInstance->GetSubsystem<UAudioSubsystem>();
        if (!AudioManager)
        {
            return;
        }

        if (bPlay2D)
        {
            AudioManager->PlaySFX(SoundType, RowName, FVector::ZeroVector);
        }
        else
        {
            const FVector PlayLocation = MeshComp->GetComponentLocation() + LocationOffset;
            AudioManager->PlaySFX(SoundType, RowName, PlayLocation);
        }
    }
    // 에디터 프리뷰
    else
    {
        const UAudioDataSettings* Ads = GetDefault<UAudioDataSettings>();
        if (!Ads)
        {
            return;
        }

        UDataTable* TargetTable = nullptr;

        switch (SoundType)
        {
        case ESfxSoundType::UI:
            if (!Ads->UISfxSounds.IsNull())
            {
                TargetTable = Ads->UISfxSounds.LoadSynchronous();
            }
            break;
        case ESfxSoundType::Character:
            if (!Ads->CharacterSounds.IsNull())
            {
                TargetTable = Ads->CharacterSounds.LoadSynchronous();
            }
            break;
        default:
            break;
        }

        if (!TargetTable)
        {
            return;
        }

        USoundBase* LoadedSound = nullptr;

        if (SoundType == ESfxSoundType::UI)
        {
            const FUISFXAudioDataStruct* Row = TargetTable->FindRow<FUISFXAudioDataStruct>(RowName, TEXT("Notify_PlaySFX(UI)"));
            if (Row && !Row->Sound.IsNull())
            {
                LoadedSound = Row->Sound.LoadSynchronous();
            }
        }
        else if (SoundType == ESfxSoundType::Character)
        {
            const FCharacterAudioDataStruct* Row = TargetTable->FindRow<FCharacterAudioDataStruct>(RowName, TEXT("Notify_PlaySFX(Character)"));
            if (Row && !Row->Sound.IsNull())
            {
                LoadedSound = Row->Sound.LoadSynchronous();
            }
        }

        if (!LoadedSound)
        {
            return;
        }

        if (bPlay2D)
        {
            UGameplayStatics::PlaySound2D(World, LoadedSound, 1.0f);
        }
        else
        {
            const FVector PlayLocation = MeshComp->GetComponentLocation() + LocationOffset;
            UGameplayStatics::PlaySoundAtLocation(World, LoadedSound, PlayLocation, 1.0f);
        }
    }
}
