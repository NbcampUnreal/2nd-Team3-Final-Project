
#include "SmallWidgetTriggerBox.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"

ASmallWidgetTriggerBox::ASmallWidgetTriggerBox()
{
	OnActorBeginOverlap.AddDynamic(this, &ASmallWidgetTriggerBox::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ASmallWidgetTriggerBox::OnOverlapEnd);
}

void ASmallWidgetTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	if (TutorialSmallWidgetClass)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			TutorialSmallWidget = CreateWidget<UUserWidget>(PC, TutorialSmallWidgetClass);
		}
	}
}

void ASmallWidgetTriggerBox::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor->IsA(ACharacter::StaticClass()) || bHasTriggered) return;

	if (TutorialSmallWidget && !TutorialSmallWidget->IsInViewport())
	{
		TutorialSmallWidget->AddToViewport();
		bHasTriggered = true; // 한 번만 띄움
	}
}

void ASmallWidgetTriggerBox::OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor->IsA(ACharacter::StaticClass())) return;

	if (TutorialSmallWidget && TutorialSmallWidget->IsInViewport())
	{
		TutorialSmallWidget->RemoveFromParent();
		bHasTriggered = false;
	}
}