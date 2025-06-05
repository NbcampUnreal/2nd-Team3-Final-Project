// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMenuWidget.h"

#include "AI_NPC/Widget/PlayerQuestWidget.h"
#include "Quest/QuestSubsystem.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Border.h"
#include "Character/EmberCharacter.h"

void UGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AEmberCharacter* EmberCharacter = Cast<AEmberCharacter>(PC->GetCharacter()))
		{
			EmberCharacter->BindUIInput(this);
		}
	}
}
void UGameMenuWidget::UpdateQuestInfoViaWidgetTree()
{
    if (!WBP_Quest) return;

    // 중간 위젯인 QuestWidget의 내부 위젯 트리에서 "WBP_Questcontents" 검색
    if (UWidget* Found = WBP_Quest->WidgetTree->FindWidget(FName("WBP_Questcontents")))
    {
        if (UPlayerQuestWidget* WBP_Questcontents = Cast<UPlayerQuestWidget>(Found))
        {
            // 정상적으로 찾았을 경우 퀘스트 데이터 갱신
            if (UQuestSubsystem* QuestSubsystem = GetGameInstance()->GetSubsystem<UQuestSubsystem>())
            {
                FName LastQuestID;
                if (QuestSubsystem->GetLastActiveQuestID(LastQuestID))
                {
                    if (UQuestDataAsset* QuestAsset = QuestSubsystem->GetAllLoadedQuests().FindRef(LastQuestID))
                    {
                        const bool bIsComplete = QuestSubsystem->IsQuestCompleted(LastQuestID);
                        const bool bIsAccepted = QuestSubsystem->IsQuestAccepted(LastQuestID);

                        WBP_Questcontents->SetQuestInfoFromDataAsset(QuestAsset, bIsComplete, bIsAccepted);
                        return;
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("WBP_Questcontents 캐스트 실패"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("WBP_Questcontents 위젯을 찾을 수 없음"));
    }
}
