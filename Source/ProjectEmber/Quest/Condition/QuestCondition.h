#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameInstance/GameplayEventSubsystem.h"
#include "QuestCondition.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew)
class PROJECTEMBER_API UQuestCondition : public UObject
{
    GENERATED_BODY()

public:
    /**
     * 태그로 검사 ( 왠만하면 모든 처리를 태그화 해서 처리 )
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
    FGameplayTag EventTag{FGameplayTag()};

    /**
     * 조건 충족을 위한 횟수
     */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest|Condition")
    int32 RequiredCount = 1;


    /**
     * 런타임 카운트
     */
    UPROPERTY(VisibleInstanceOnly, Category = "Quest|Condition")
    int32 CurrentCount = 0;

    /**
     * 블루프린트에서도 상속 가능하게 처리
     * 태그가 일치하면 카운트 증가 후 조건 충족 여부 확인
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Quest|Condition")
    bool OnEvent(const FGameplayTag& InEventTag, const FGameplayEventData& EventData);
    virtual bool OnEvent_Implementation(const FGameplayTag& InEventTag, const FGameplayEventData& EventData)
    {
        if (InEventTag != EventTag)
        {
            return false;
        }
        CurrentCount++;
        return IsFulfilled();
    }

    /**
     * 조건 충족 여부 확인
     */
    UFUNCTION(BlueprintCallable, Category = "Quest|Condition")
    bool IsFulfilled() const
    {
        return CurrentCount >= RequiredCount;
    }

    /**
     * 반복 퀘스트 고려 리셋함수
     */
    UFUNCTION(BlueprintCallable, Category = "Quest|Condition")
    void Reset()
    {
        CurrentCount = 0;
    }
};