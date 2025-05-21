#include "GameInstance/AudioSubsystem.h"
#include "AudioDataSettings.h"
#include "AudioMixerDevice.h"
#include "Structs/AreaAudioDataStruct.h"
#include "Structs/CharacterAudioDataStruct.h"

#include "Components/AudioComponent.h"
#include "Structs/UISFXAudioDataStruct.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	AudioDataSettings = GetDefault<UAudioDataSettings>();
	LoadDataTables();
}

void UAudioSubsystem::LoadDataTables()
{
	if (AudioDataSettings)
	{
		if (!AudioDataSettings->AreaSounds.IsNull())
		{
			AreaSoundTable = AudioDataSettings->AreaSounds.LoadSynchronous();
		}

		if (!AudioDataSettings->CharacterSounds.IsNull())
		{
			CharacterSoundTable = AudioDataSettings->CharacterSounds.LoadSynchronous();
		}
	}
}

void UAudioSubsystem::PlayBGM(EAreaSoundType SoundType)
{
	if (GetWorld())
	{
		UEnum* EnumPtr = StaticEnum<EAreaSoundType>();

		if (AreaSoundTable)
		{
			FName RowName = FName(*EnumPtr->GetNameStringByValue(static_cast<int64_t>(SoundType)));

			const FAreaAudioDataStruct* FoundRow = AreaSoundTable->FindRow<FAreaAudioDataStruct>(RowName, TEXT("PlayBGM AudioSubsystem"));
			
			if (FoundRow)
			{
				if (!FoundRow->Sound.IsNull())
				{
					USoundBase* Sound = FoundRow->Sound.LoadSynchronous();

					if (Sound)
					{
						if (BgmComp && BgmComp->IsPlaying())
						{
							BgmComp->Stop();
							EMBER_LOG(LogTemp, Warning, TEXT("Stop BGM Sound"));
						}

						BgmComp = UGameplayStatics::CreateSound2D(GetWorld(), Sound, MasterVolume);

						if (BgmComp)
						{
							BgmComp->Play();
							EMBER_LOG(LogTemp, Warning, TEXT("Play BGM Sound"));
						}
					}
				}
			}
		}
	}
}

void UAudioSubsystem::PlaySFX(ESfxSoundType SoundType, const FName RowName, FVector Location)
{
	switch (SoundType)
	{
	case ESfxSoundType::UI:
		PlaySFXByRowName<FUISFXAudioDataStruct>(GetWorld(), UISfxSoundTable, RowName, FVector::ZeroVector);
		break;
	case ESfxSoundType::Character:
		PlaySFXByRowName<FCharacterAudioDataStruct>(GetWorld(), CharacterSoundTable, RowName, Location);
		break;
	}
}

void UAudioSubsystem::SetAndApplyMasterVolume(float NewVolume)
{
	MasterVolume = FMath::Clamp(NewVolume, 0.0f, 1.0f);

	if (BgmComp)
	{
		if (MasterVolume != 0.0f)
		{
			BgmComp->SetVolumeMultiplier(MasterVolume);
			BgmComp->SetPaused(false);
		}
		else
		{
			BgmComp->SetPaused(true);
		}
	}
}

void UAudioSubsystem::PlayBGMSoundByArea(EAreaType Type)
{
	EAreaSoundType SoundType = EAreaSoundType::LobbySound;

	switch (Type)
	{
	case EAreaType::LobbyArea:
		SoundType = EAreaSoundType::LobbySound;
		break;
		
	case EAreaType::GrasslandArea:
		SoundType = EAreaSoundType::GrasslandSound;
		break;

	case EAreaType::DesertArea:
		SoundType = EAreaSoundType::DesertSound;
		break;
	}

	PlayBGMSound(SoundType);
}

void UAudioSubsystem::PlayBGMSound(EAreaSoundType Type)
{
	if (!CheckValidOfBgmSource(Type))
	{
		return;
	}

	if (!IsValid(BgmSoundMap[Type]))
	{
		return;
	}

	if (!IsValid(BgmComp))
	{
		BgmComp = UGameplayStatics::CreateSound2D(GetGameInstance(), BgmSoundMap[Type], MasterVolume * BgmVolume);
		BgmComp->bAutoDestroy = false;
	}

	if (BgmComp->IsPlaying())
	{
		BgmComp->Stop();
	}

	BgmComp->SetSound(BgmSoundMap[Type]);
	BgmComp->SetVolumeMultiplier(MasterVolume * BgmVolume);
	BgmComp->Play();
}

void UAudioSubsystem::SetBgmVolume(float VolumeValue)
{
	BgmVolume = FMath::Clamp(VolumeValue, 0.0f, 1.0f);

	if (IsValid(BgmComp))
	{
		BgmComp->SetVolumeMultiplier(MasterVolume * BgmVolume);
	}
}

void UAudioSubsystem::SetEffectsVolume(float VolumeValue)
{
	EffectsVolume = FMath::Clamp(VolumeValue, 0.0f, 1.0f);
}

bool UAudioSubsystem::CheckValidOfBgmSource(EAreaSoundType SoundType)
{
	if (BgmSoundMap.Contains(SoundType))
	{
		return true;
	}

	if (!CheckValidOfBgmAudio())
	{
		return false;
	}

	USoundBase* SoundSource = nullptr;

	for (FAreaAudioDataStruct* SoundData : SoundDataArraySet.AreaAudioArray)
	{
		if (SoundData->AreaSoundType == SoundType)
		{
			SoundSource = SoundData->Sound.LoadSynchronous();
			break;
		}
	}

	if (!IsValid(SoundSource))
	{
		return false;
	}

	BgmSoundMap.Add(SoundType, SoundSource);

	return true;
}

bool UAudioSubsystem::CheckValidOfBgmAudio()
{
	if (SoundDataArraySet.AreaAudioArray.IsEmpty())
	{
		if (!IsValid(AudioDataSettings))
		{
			return false;
		}

		if (AudioDataSettings->AreaSounds.IsNull())
		{
			return false;
		}

		const FString DataContext(TEXT("Data Context"));

		AreaSoundTable = AudioDataSettings->AreaSounds.LoadSynchronous();
		AreaSoundTable->GetAllRows(DataContext, SoundDataArraySet.AreaAudioArray);
	}

	return true;
}

float UAudioSubsystem::GetBgmVolume() const
{
	return BgmVolume;
}
float UAudioSubsystem::GetMasterVolume() const
{
	return MasterVolume;
}
float UAudioSubsystem::GetEffectsVolume() const
{
	return EffectsVolume;
}

bool UAudioSubsystem::CheckValidOfCharacterAudio()
{
	if (SoundDataArraySet.CharacterAudioArray.IsEmpty())
	{
		if (!IsValid(AudioDataSettings))
		{
			return false;
		}

		if (AudioDataSettings->CharacterSounds.IsNull())
		{
			return false;
		}

		const FString DataContext(TEXT("Data Context"));

		CharacterSoundTable = AudioDataSettings->CharacterSounds.LoadSynchronous();
		CharacterSoundTable->GetAllRows(DataContext, SoundDataArraySet.CharacterAudioArray);
	}
	
	return true;
}