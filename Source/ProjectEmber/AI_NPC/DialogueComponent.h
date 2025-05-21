// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
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


    UFUNCTION()
    void PositionDetachedCamera();

    void AdvanceDialogue();
    void Interact();
    void ShowQuestUI();
    void LoadDialogueFromDataTable(bool bResetDialogueIndex = true);
    void SetInputMappingContext(UInputMappingContext* MappingContext, bool bClearExisting = true);

    UPROPERTY()
    bool bDialogueFinished = false;
    bool IsDialogueActive() const;
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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
    UDataTable* QuestDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
    FName QuestRowName = FName("MainQuest01");


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> LinesOfDialogue;

    UPROPERTY(VisibleAnywhere, Category = "Dialogue")
    int32 CurrentDialogueIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TSubclassOf<UUserWidget> QuestWidgetClass;

    UPROPERTY(EditAnywhere, Category = "Dialogue")
    UDataTable* DialogueDataTable;

    UPROPERTY(EditAnywhere, Category = "Dialogue")
    FName DialogueRowName;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* UIInputMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* GameplayInputMappingContext;


  

};