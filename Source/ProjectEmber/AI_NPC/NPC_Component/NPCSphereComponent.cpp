#include "NPCSphereComponent.h"

UNPCSphereComponent::UNPCSphereComponent()
{
    InitSphereRadius(150.f);
    SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    SetGenerateOverlapEvents(true);
}