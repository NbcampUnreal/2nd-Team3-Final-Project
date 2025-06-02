#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ThrowFromSocket.generated.h"

UCLASS(meta=(DisplayName="Throw From Socket"))
class EMBERABILITYSYSTEM_API UAnimNotify_ThrowFromSocket : public UAnimNotify
{
	GENERATED_BODY()
public:
	// 어느 소켓에서 스폰할 건지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Throw")
	FName SocketName = NAME_None;

	// 던질 액터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Throw")
	TSubclassOf<AActor> ProjectileClass;

	// 던질 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Throw")
	float ThrowSpeed = 2000.f;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
