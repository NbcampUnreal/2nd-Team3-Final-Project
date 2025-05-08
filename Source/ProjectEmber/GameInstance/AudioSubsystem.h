#pragma once

#include "CoreMinimal.h"
#include "EnumSet.h"
#include "GameInstance/Structs/SoundDataArrayStruct.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSubsystem.generated.h"

class UAudioDataSettings;

UCLASS()
class PROJECTEMBER_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void LoadDataTables();

	void PlayBGM(EAreaSoundType SoundType);

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetAndApplyMasterVolume(float NewVolume);

	void PlayBGMSoundByArea(EAreaType Type);
	void PlayBGMSound(EAreaSoundType Type);

	const float GetBgmVolume() const;

	void SetBgmVolume(float VolumeValue);
	
	bool CheckValidOfBgmSource(EAreaSoundType SoundType);
	
	bool CheckValidOfBgmAudio();
	//bool CheckValidOfUIAudio();
	bool CheckValidOfCharacterAudio();
	//bool CheckValidOfBossAudio();
	//bool CheckValidOfMonsterAudio();

private:
	UPROPERTY()
	FSoundDataArrayStruct SoundDataArraySet;

	UPROPERTY()
	TMap<EAreaSoundType, USoundBase*> BgmSoundMap;

	//UPROPERTY()
	//TMap<EUISfxSoundType, USoundBase*> UISoundMap;

	UPROPERTY()
	UDataTable* AreaSoundTable;

	UPROPERTY()
	UDataTable* CharacterSoundTable;

	UPROPERTY()
	TObjectPtr<UAudioComponent> BgmComp;

	UPROPERTY()
	const UAudioDataSettings* AudioDataSettings;

	UPROPERTY()
	float MasterVolume = 1.0f;

	UPROPERTY()
	float BgmVolume = 0.5f;
};
