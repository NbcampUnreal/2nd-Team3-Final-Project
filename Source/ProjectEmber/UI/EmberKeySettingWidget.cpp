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

void UEmberKeySettingWidget::RefreshKeyList()
{
    KeyListContainer->ClearChildren();
    KeyButtonMap.Empty();

    for (int32 i = 0; i < CurrentMoveMappings.Num(); ++i)
    {
        const auto& Entry = CurrentMoveMappings[i];

        UHorizontalBox* Row = NewObject<UHorizontalBox>(this);

        UTextBlock* Label = NewObject<UTextBlock>(this);
        Label->SetText(FText::FromString(MoveDirectionToString(Entry.Direction)));

        UButton* KeyButton = NewObject<UButton>(this);
        UTextBlock* KeyLabel = NewObject<UTextBlock>(this);
        KeyLabel->SetText(FText::FromName(Entry.BoundKey.GetFName()));
        KeyButton->AddChild(KeyLabel);

        KeyButton->OnClicked.AddDynamic(this, &UEmberKeySettingWidget::OnAnyKeyButtonClicked);
        KeyButtonMap.Add(KeyButton, i);

        Row->AddChildToHorizontalBox(Label);
        Row->AddChildToHorizontalBox(KeyButton);
        KeyListContainer->AddChild(Row);
    }

    UHorizontalBox* BottomRow = NewObject<UHorizontalBox>(this);

    UButton* DynamicApplyButton = NewObject<UButton>(this);
    UTextBlock* ApplyText = NewObject<UTextBlock>(this);
    ApplyText->SetText(FText::FromString(TEXT("Apply")));
    DynamicApplyButton->AddChild(ApplyText);
    DynamicApplyButton->OnClicked.AddDynamic(this, &UEmberKeySettingWidget::OnApplyClicked);

    UButton* DynamicResetButton = NewObject<UButton>(this);
    UTextBlock* ResetText = NewObject<UTextBlock>(this);
    ResetText->SetText(FText::FromString(TEXT("Reset")));
    DynamicResetButton->AddChild(ResetText);
    DynamicResetButton->OnClicked.AddDynamic(this, &UEmberKeySettingWidget::OnResetClicked);

    BottomRow->AddChildToHorizontalBox(DynamicApplyButton);
    BottomRow->AddChildToHorizontalBox(DynamicResetButton);
    KeyListContainer->AddChild(BottomRow);
}

void UEmberKeySettingWidget::OnAnyKeyButtonClicked()
{
    for (const auto& Pair : KeyButtonMap)
    {
        if (Pair.Key->HasUserFocus(GetOwningPlayer()))
        {
            OnKeyButtonClicked(Pair.Value);
            return;
        }
    }
}

void UEmberKeySettingWidget::OnKeyButtonClicked(int32 MappingIndex)
{
    if (!KeyCaptureWidgetClass) return;

    PendingKeyChangeIndex = MappingIndex;

    UEmberKeyCaptureWidget* CaptureWidget = CreateWidget<UEmberKeyCaptureWidget>(this, KeyCaptureWidgetClass);
    if (CaptureWidget)
    {
        CaptureWidget->OnKeyCaptured.AddDynamic(this, &UEmberKeySettingWidget::OnKeyCaptured);
        CaptureWidget->AddToViewport();
    }
}

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

void UEmberKeySettingWidget::OnKeyCaptured(FKey NewKey)
{
    if (!NewKey.IsValid() || !CurrentMoveMappings.IsValidIndex(PendingKeyChangeIndex))
        return;

    for (int32 i = 0; i < CurrentMoveMappings.Num(); ++i)
    {
        if (i != PendingKeyChangeIndex && CurrentMoveMappings[i].BoundKey == NewKey)
        {
            UE_LOG(LogTemp, Warning, TEXT("[KeySetting] Key '%s' is already used by another direction!"), *NewKey.GetDisplayName().ToString());
            return;
        }
    }

    CurrentMoveMappings[PendingKeyChangeIndex].BoundKey = NewKey;
    RefreshKeyList();
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
    RefreshKeyList();
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
    RefreshKeyList();
}