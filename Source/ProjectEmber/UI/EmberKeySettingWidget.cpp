#include "EmberKeySettingWidget.h"
#include "EmberKeyCaptureWidget.h"
#include "Character/EmberCharacter.h"
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

    if (JumpKeyButton)
        JumpKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnJumpKeyButtonClicked);

    if (GlideKeyButton)
        GlideKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnGlideKeyButtonClicked);

    if (SprintKeyButton)
        SprintKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnSprintKeyButtonClicked);

    if (WalkKeyButton)
        WalkKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnWalkKeyButtonClicked);

    if (CrouchKeyButton)
        CrouchKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnCrouchKeyButtonClicked);

    if (BuildKeyButton)
        BuildKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnBuildKeyButtonClicked);

    if (InteractKeyButton)
        InteractKeyButton->OnClicked.AddDynamic(this, &ThisClass::OnInteractKeyButtonClicked);

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

/*
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
}*/

FString UEmberKeySettingWidget::GetMoveDirectionMappingName(EMoveDirection Dir)
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

void UEmberKeySettingWidget::OnJumpKeyButtonClicked()
{
    PendingKeyChangeIndex = FindMappingIndexByName("IA_Als_Jump");
    PendingKeyChangeDirection = EMoveDirection::Unknown;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnGlideKeyButtonClicked()
{
    PendingKeyChangeIndex = FindMappingIndexByName("IA_Als_Glide");
    PendingKeyChangeDirection = EMoveDirection::Unknown;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnSprintKeyButtonClicked()
{
    PendingKeyChangeIndex = FindMappingIndexByName("IA_Als_Sprint");
    PendingKeyChangeDirection = EMoveDirection::Unknown;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnWalkKeyButtonClicked()
{
    PendingKeyChangeIndex = FindMappingIndexByName("IA_Als_Walk");
    PendingKeyChangeDirection = EMoveDirection::Unknown;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnCrouchKeyButtonClicked()
{
    PendingKeyChangeIndex = FindMappingIndexByName("IA_Als_Crouch");
    PendingKeyChangeDirection = EMoveDirection::Unknown;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnBuildKeyButtonClicked()
{
    PendingKeyChangeIndex = FindMappingIndexByName("IA_Als_Build");
    PendingKeyChangeDirection = EMoveDirection::Unknown;
    ShowKeyCaptureWidget();
}

void UEmberKeySettingWidget::OnInteractKeyButtonClicked()
{
    PendingKeyChangeIndex = FindMappingIndexByName("IA_Interact");
    PendingKeyChangeDirection = EMoveDirection::Unknown;
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
    if (PendingKeyChangeDirection != EMoveDirection::Unknown)
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
    else if (PendingKeyChangeIndex >= 0 && PendingKeyChangeIndex < CurrentMappings.Num())
    {
        CurrentMappings[PendingKeyChangeIndex].BoundKey = NewKey;
        UpdateActionKeyText(CurrentMappings[PendingKeyChangeIndex].MappingName);
    }

}

void UEmberKeySettingWidget::LoadMappingsFromSave()
{
    CurrentMoveMappings.Empty();
    CurrentMappings.Empty();

    UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>();
    if (GI) GI->LoadKeyMappingsWithEMS();

    // --- MOVE (방향키) ---
    if (GI && GI->SavedMoveBindings.Num() > 0)
    {
        CurrentMoveMappings = GI->SavedMoveBindings;
    }
    else
    {
        if (EditableIMC)
        {
            ClonedIMC = DuplicateObject<UInputMappingContext>(EditableIMC, this);
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

    // --- ACTION KEY ---
    static const TArray<FName> ActionNames = {
        "IA_Als_Jump", "IA_Als_Sprint", "IA_Als_Walk", "IA_Als_Crouch", "IA_Als_Glide", "IA_Als_Build", "IA_Interact"
    };

    if (GI && GI->SavedMappings.Num() > 0)
    {
        CurrentMappings = GI->SavedMappings;
        // 반드시 BoundKey로 UI 업데이트!
        for (const FEmberKeyMappingEntry& Entry : CurrentMappings)
        {
            UpdateActionKeyText(Entry.MappingName);
        }
    }
    else
    {
        UInputMappingContext* SourceIMC = EditableIMC ? EditableIMC : (GI ? GI->PlayerMappingContext.Get() : nullptr);
        if (SourceIMC)
        {
            for (const FEnhancedActionKeyMapping& Mapping : SourceIMC->GetMappings())
            {
                if (Mapping.Action && ActionNames.Contains(Mapping.Action->GetFName()))
                {
                    FEmberKeyMappingEntry Entry;
                    Entry.MappingName = Mapping.Action->GetFName();
                    Entry.BoundKey = Mapping.Key;
                    Entry.DefaultKey = Mapping.Key;
                    Entry.InputAction = Mapping.Action;
                    CurrentMappings.Add(Entry);
                }
            }
            for (const FEmberKeyMappingEntry& Entry : CurrentMappings)
            {
                UpdateActionKeyText(Entry.MappingName);
            }
        }
    }

    if (GI && GI->SavedMappings.Num() > 0)
    {
        CurrentMappings = GI->SavedMappings;

        UE_LOG(LogTemp, Warning, TEXT("=== CurrentMappings (after load) ==="));
        for (const auto& Entry : CurrentMappings)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *Entry.MappingName.ToString(), *Entry.BoundKey.GetFName().ToString());
        }

        for (const FEmberKeyMappingEntry& Entry : CurrentMappings)
        {
            UpdateActionKeyText(Entry.MappingName);
        }
    }

    UpdateKeyUI(); // 방향키도 UI 갱신
}

/*void UEmberKeySettingWidget::LoadMappingsFromSave()
{
    CurrentMoveMappings.Empty();
    CurrentMappings.Empty();

    UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>();

    UInputMappingContext* SourceIMC = nullptr;
    if (EditableIMC)
        SourceIMC = EditableIMC;
    else if (GI)
        SourceIMC = GI->PlayerMappingContext.Get();
    if (!SourceIMC) return;

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

    CurrentMappings.Empty();
    static const TArray<FName> ActionNames = {
        "IA_Als_Jump", "IA_Als_Sprint", "IA_Als_Walk", "IA_Als_Crouch", "IA_Als_Glide", "IA_Als_Build", "IA_Interact"
    };
    if (GI && GI->SavedMappings.Num() > 0)
    {
        CurrentMappings = GI->SavedMappings;
        for (const FEmberKeyMappingEntry& Entry : CurrentMappings)
        {
            UpdateActionKeyText(Entry.MappingName);
        }
    }
    else
    {
        for (const FEnhancedActionKeyMapping& Mapping : SourceIMC->GetMappings())
        {
            if (Mapping.Action && ActionNames.Contains(Mapping.Action->GetFName()))
            {
                FEmberKeyMappingEntry Entry;
                Entry.MappingName = Mapping.Action->GetFName();
                Entry.BoundKey = Mapping.Key;
                Entry.DefaultKey = Mapping.Key;
                Entry.InputAction = Mapping.Action;
                CurrentMappings.Add(Entry);

                UpdateActionKeyText(Entry.MappingName);
            }
        }
    }

    UpdateKeyUI();
}*/

void UEmberKeySettingWidget::OnApplyClicked()
{
    for (const auto& Entry : CurrentMappings)
    {
        UE_LOG(LogTemp, Warning, TEXT("[APPLY] %s = %s"), *Entry.MappingName.ToString(), *Entry.BoundKey.GetFName().ToString());
    }

    UEmberGameInstance* GI = GetWorld()->GetGameInstance<UEmberGameInstance>();
    if (GI) {
        GI->SavedMoveBindings = CurrentMoveMappings;
        GI->SavedMappings = CurrentMappings;
        GI->SaveKeyMappingsWithEMS();
    }

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

    for (const auto& Entry : CurrentMappings)
    {
        FMapPlayerKeyArgs Args;
        Args.MappingName = Entry.MappingName;
        Args.NewKey = Entry.BoundKey;
        Args.Slot = EPlayerMappableKeySlot::First;
        FGameplayTagContainer FailureReason;
        UserSettings->MapPlayerKey(Args, FailureReason);
    }
    UserSettings->SaveSettings();

}

void UEmberKeySettingWidget::OnResetClicked()
{
    for (FEmberDirectionalMoveEntry& Entry : CurrentMoveMappings)
    {
        Entry.BoundKey = Entry.DefaultKey;
    }
    LoadMappingsFromSave();
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

void UEmberKeySettingWidget::UpdateActionKeyText(FName MappingName)
{
    FKey Key = GetKeyForMapping(MappingName);

    UE_LOG(LogTemp, Warning, TEXT("UpdateActionKeyText %s : %s"), *MappingName.ToString(), *Key.GetFName().ToString());

    if (MappingName == "IA_Als_Jump" && JumpButtonText)
    {
        JumpButtonText->SetText(FText::FromName(Key.GetFName()));
    }
    else if (MappingName == "IA_Als_Glide" && GlideButtonText)
    {
        GlideButtonText->SetText(FText::FromName(Key.GetFName()));
    }
    else if (MappingName == "IA_Als_Sprint" && SprintButtonText)
    {
        SprintButtonText->SetText(FText::FromName(Key.GetFName()));
    }
    else if (MappingName == "IA_Als_Walk" && WalkButtonText)
    {
        WalkButtonText->SetText(FText::FromName(Key.GetFName()));
    }
    else if (MappingName == "IA_Als_Crouch" && CrouchButtonText)
    {
        CrouchButtonText->SetText(FText::FromName(Key.GetFName()));
    }
    else if (MappingName == "IA_Als_Build" && BuildButtonText)
    {
        BuildButtonText->SetText(FText::FromName(Key.GetFName()));
    }
    else if (MappingName == "IA_Interact" && InteractButtonText)
    {
        InteractButtonText->SetText(FText::FromName(Key.GetFName()));
    }
}

FKey UEmberKeySettingWidget::GetKeyForMapping(FName MappingName) const
{
    for (const FEmberKeyMappingEntry& Entry : CurrentMappings)
    {
        if (Entry.MappingName == MappingName)
            return Entry.BoundKey;
    }
    return EKeys::Invalid;
}

int32 UEmberKeySettingWidget::FindMappingIndexByName(FName MappingName) const
{
    for (int32 i = 0; i < CurrentMappings.Num(); ++i)
    {
        if (CurrentMappings[i].MappingName == MappingName)
            return i;
    }
    return INDEX_NONE;
}