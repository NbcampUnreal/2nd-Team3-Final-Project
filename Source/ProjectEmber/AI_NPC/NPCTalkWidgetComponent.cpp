#include "NPCTalkWidgetComponent.h"

UNPCTalkWidgetComponent::UNPCTalkWidgetComponent()
{
    SetWidgetSpace(EWidgetSpace::Screen);
    SetDrawSize(FVector2D(300.f, 100.f));
    SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetVisibility(false);
    SetRelativeLocation(FVector(0.f, 0.f, 180.f));
}