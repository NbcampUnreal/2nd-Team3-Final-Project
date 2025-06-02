#include "NPCTalkWidgetComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"

UNPCTalkWidgetComponent::UNPCTalkWidgetComponent()
{
    SetWidgetSpace(EWidgetSpace::Screen);
    SetDrawSize(FVector2D(300.f, 100.f));
    SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetRelativeLocation(FVector(0.f, 0.f, 180.f));
    SetVisibility(false); 
}

void UNPCTalkWidgetComponent::OnRegister()
{
    Super::OnRegister();

  
    if (UUserWidget* WidgetInstance = GetUserWidgetObject())
    {
        WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
    }

    SetVisibility(false); 
}