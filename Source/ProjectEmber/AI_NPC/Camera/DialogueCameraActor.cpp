#include "DialogueCameraActor.h"
#include "CineCameraComponent.h" 


ADialogueCameraActor::ADialogueCameraActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    GetCineCameraComponent()->SetFieldOfView(45.f);
}