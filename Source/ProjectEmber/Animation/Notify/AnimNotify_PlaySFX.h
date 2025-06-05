#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameInstance/EnumSet.h"
#include "AnimNotify_PlaySFX.generated.h"


UCLASS()
class PROJECTEMBER_API UAnimNotify_PlaySFX : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	ESfxSoundType SoundType = ESfxSoundType::Character;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	FName RowName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	bool bPlay2D = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (EditCondition = "!bPlay2D"))
	FVector LocationOffset = FVector::ZeroVector;
};
