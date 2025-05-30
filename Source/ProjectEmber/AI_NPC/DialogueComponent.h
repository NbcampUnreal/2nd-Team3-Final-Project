// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "Quest/Data/QuestDataAsset.h"
#include "Interactables/Interactable.h"
#include "DialogueComponent.generated.h"

class UNPCSphereComponent;
class UNPCTalkWidgetComponent;
class ADialogueCameraActor;
class UInputMappingContext;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTEMBER_API UDialogueComponent : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

public:
    UDialogueComponent();
    virtual void Interact_Implementation(AActor* Caller) override;
    virtual float GetGatherTime_Implementation() override;

    UFUNCTION()
    void RepositionNPCForDialogue();

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void UpdateQuestLogWidgetFromAsset(const UQuestDataAsset* InQuestAsset);

    UFUNCTION()
    void PositionDetachedCamera();
    void ShowQuestCompleteWidget(const UQuestDataAsset* InQuestAsset);
    void AdvanceDialogue();
    void Interact();
    void ShowQuestUI();
    void LoadDialogueFromDataTable(bool bResetDialogueIndex, FName InObjectiveTag = NAME_None);
    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetInputMappingContexts(TArray<UInputMappingContext*> MappingContexts, bool bClearExisting = true);

    UPROPERTY()
    bool bDialogueFinished = false;
    bool IsDialogueActive() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
    UQuestDataAsset* QuestAsset;

protected:
    virtual void BeginPlay() override;
    UFUNCTION()
    void OnPlayerEnterRadius(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPlayerExitRadius(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UPROPERTY()
    bool bPlayerInRange = false;

    UPROPERTY()
    UNPCSphereComponent* InteractionTrigger;

    UPROPERTY()
    UNPCTalkWidgetComponent* TalkPromptWidget;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> TalkPromptWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> DialogueWidgetClass;

    UPROPERTY()
    UUserWidget* DialogueWidget;

    UPROPERTY(EditAnywhere, Category = "Dialogue|Camera")
    ADialogueCameraActor* DialogueCameraActor;


    UPROPERTY(EditDefaultsOnly, Category = "Quest")
    TSubclassOf<class UQuestWidget> QuestCompleteWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FText> LinesOfDialogue;

    UPROPERTY(VisibleAnywhere, Category = "Dialogue")
    int32 CurrentDialogueIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSubclassOf<UUserWidget> QuestWidgetClass;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* UIInputMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* GameplayInputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* GameplayUIInputMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> DialogueRowNames;

    UFUNCTION()
    void SetDialogueVisualState(bool bShowUI);
  
    UFUNCTION()
    void InitializeAndDisplayWidget(UUserWidget* Widget);

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHUDClass;

    UPROPERTY()
    UUserWidget* PlayerHUD;

    UPROPERTY()
    UQuestWidget* QuestWidgetInstance;



};