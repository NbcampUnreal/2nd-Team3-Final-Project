#include "EmberKeySettingWidget.h"
#include "EmberKeyCaptureWidget.h"
#include "GameInstance/EmberGameInstance.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "Character/InputHandler/EmberKeyMappingEntry.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "InputTriggers.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"

void UEmberKeySettingWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ForwardKeyButton)
        ForwardKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnForwardKeyButtonClicked);

    if (BackwardKeyButton)
        BackwardKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnBackwardKeyButtonClicked);

    if (LeftwardKeyButton)
        LeftwardKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnLeftwardKeyButtonClicked);

    if (RightwardKeyButton)
        RightwardKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnRightwardKeyButtonClicked);

    if (ApplyButton)
        ApplyButton->OnClicked.AddDynamic(this, &ThisClass::OnApplyClicked);

    if (ResetButton)
        ResetButton->OnClicked.AddDynamic(this, &ThisClass::OnResetClicked);

    LoadMappingsFromSave();
}

EMoveDirection UEmberKeySettingWidget::GetDirectionFromModifiers(const TArray<TObjectPtr<UInputModifier>>& Modifiers)
{
    if (Modifiers.Num() == 1)
    {
        FString ModName = Modifiers[0]->GetClass()->GetName();
        if (ModName.Contains(TEXT("Swizzle")) || ModName.Contains(TEXT("InputModifierSwizzleAxis")))
            return EMoveDirection::Forward;
        if (ModName.Contains(TEXT("Negate")))
            return EMoveDirection::Left;
    }
    else if (Modifiers.Num() == 2)
    {
        bool bHasSwizzle = false, bHasNegate = false;
        for (UInputModifier* Mod : Modifiers)
        {
            FString ModName = Mod->GetClass()->GetName();
            if (ModName.Contains(TEXT("Swizzle")) || ModName.Contains(TEXT("InputModifierSwizzleAxis")))
                bHasSwizzle = true;
            if (ModName.Contains(TEXT("Negate")))
                bHasNegate = true;
        }
        if (bHasSwizzle && bHasNegate)
            return EMoveDirection::Backward;
    }
    else if (Modifiers.Num() == 0)
    {
        return EMoveDirection::Right;
    }
    return EMoveDirection::Unknown;
}

void UEmberKeySettingWidget::SetTargetMappingContext(UInputMappingContext* InContext)
{
    TargetMappingContext = InContext;
    CurrentMoveMappings.Empty();

    UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>();
    if (!GI) return;

    for (const FEnhancedActionKeyMapping& Mapping : TargetMappingContext->GetMappings())
    {
        if (Mapping.Action && GI->MoveAction && Mapping.Action == GI->MoveAction)
        {
            EMoveDirection Dir = GetDirectionFromModifiers(Mapping.Modifiers);
            if (Dir != EMoveDirection::Unknown)
            {
                FEmberDirectionalMoveEntry MoveEntry;
                MoveEntry.Direction = Dir;
                MoveEntry.BoundKey = Mapping.Key;
                MoveEntry.DefaultKey = Mapping.Key;
                CurrentMoveMappings.Add(MoveEntry);
            }
        }
    }
}

static FString MoveDirectionToString(EMoveDirection Dir)
{
    switch (Dir)
    {
    case EMoveDirection::Forward:  return TEXT("Forward");
    case EMoveDirection::Backward: return TEXT("Backward");
    case EMoveDirection::Left:     return TEXT("Left");
    case EMoveDirection::Right:    return TEXT("Right");
    default:                       return TEXT("Unknown");
    }
}

static FString GetMoveDirectionMappingName(EMoveDirection Dir)
{
    switch (Dir)
    {
    case EMoveDirection::Forward:  return TEXT("MoveForward");
    case EMoveDirection::Backward: return TEXT("MoveBackward");
    case EMoveDirection::Left:     return TEXT("MoveLeft");
    case EMoveDirection::Right:    return TEXT("MoveRight");
    default:                       return TEXT("");
    }
}

void UEmberKeySettingWidget::OnForwardKeyButtonClicked()
{
    PendingKeyChangeDirection = EMoveDirection::Forward;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnBackwardKeyButtonClicked()
{
    PendingKeyChangeDirection = EMoveDirection::Backward;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnLeftwardKeyButtonClicked()
{
    PendingKeyChangeDirection = EMoveDirection::Left;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnRightwardKeyButtonClicked()
{
    PendingKeyChangeDirection = EMoveDirection::Right;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::ShowKeyCaptureWidget()
{
    if (!KeyCaptureWidgetClass) return;

    UEmberKeyCaptureWidget* CaptureWidget = CreateWidget<UEmberKeyCaptureWidget>(this, KeyCaptureWidgetClass);
    if (CaptureWidget)
    {
        CaptureWidget->OnKeyCaptured.AddDynamic(this, &UEmberKeySettingWidget::OnKeyCaptured);
        CaptureWidget->AddToViewport();
    }
}

void UEmberKeySettingWidget::OnKeyCaptured(FKey NewKey)
{
    for (auto& Entry : CurrentMoveMappings)
    {
        if (Entry.Direction == PendingKeyChangeDirection)
        {
            Entry.BoundKey = NewKey;
            break;
        }
    }
    UpdateKeyUI();
}

void UEmberKeySettingWidget::LoadMappingsFromSave()
{
    UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>();
    if (GI && GI->SavedMoveBindings.Num() > 0)
    {
        CurrentMoveMappings = GI->SavedMoveBindings;
    }
    else
    {
        if (EditableIMC)
        {
            ClonedIMC = DuplicateObject<UInputMappingContext>(EditableIMC, this);

            CurrentMoveMappings.Empty();
            for (const FEnhancedActionKeyMapping& Mapping : ClonedIMC->GetMappings())
            {
                if (GI && Mapping.Action == GI->MoveAction)
                {
                    EMoveDirection Dir = GetDirectionFromModifiers(Mapping.Modifiers);
                    if (Dir != EMoveDirection::Unknown)
                    {
                        FEmberDirectionalMoveEntry MoveEntry;
                        MoveEntry.Direction = Dir;
                        MoveEntry.BoundKey = Mapping.Key;
                        MoveEntry.DefaultKey = Mapping.Key;
                        CurrentMoveMappings.Add(MoveEntry);
                    }
                }
            }
        }
    }
    UpdateKeyUI();
}

void UEmberKeySettingWidget::OnApplyClicked()
{
    UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>();
    if (!GI) return;

    GI->SavedMoveBindings = CurrentMoveMappings;

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    ULocalPlayer* LP = PC->GetLocalPlayer();
    if (!LP) return;

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP);
    if (!Subsystem) return;

    UEnhancedInputUserSettings* UserSettings = Subsystem->GetUserSettings();
    if (!UserSettings) return;

    for (const auto& MoveEntry : CurrentMoveMappings)
    {
        FMapPlayerKeyArgs Args;
        Args.MappingName = FName(GetMoveDirectionMappingName(MoveEntry.Direction));
        Args.Slot = EPlayerMappableKeySlot::First;
        Args.NewKey = MoveEntry.BoundKey;

        FGameplayTagContainer FailureReason;
        UserSettings->MapPlayerKey(Args, FailureReason);
    }
}

void UEmberKeySettingWidget::OnResetClicked()
{
    for (FEmberDirectionalMoveEntry& Entry : CurrentMoveMappings)
    {
        Entry.BoundKey = Entry.DefaultKey;
    }
    UpdateKeyUI();
}

void UEmberKeySettingWidget::UpdateKeyUI()
{
    for (const auto& Entry : CurrentMoveMappings)
    {
        switch (Entry.Direction)
        {
        case EMoveDirection::Forward:
            if (ForwardButtonText) ForwardButtonText->SetText(FText::FromName(Entry.BoundKey.GetFName()));
            break;
        case EMoveDirection::Backward:
            if (BackwardButtonText) BackwardButtonText->SetText(FText::FromName(Entry.BoundKey.GetFName()));
            break;
        case EMoveDirection::Left:
            if (LeftwardButtonText) LeftwardButtonText->SetText(FText::FromName(Entry.BoundKey.GetFName()));
            break;
        case EMoveDirection::Right:
            if (RightwardButtonText) RightwardButtonText->SetText(FText::FromName(Entry.BoundKey.GetFName()));
            break;
        }
    }
}