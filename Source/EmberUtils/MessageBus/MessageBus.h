#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MessageBus.generated.h"

DECLARE_DELEGATE_TwoParams(FMessageDelegate, const FName, UObject*);

/** MessageBus 클래스
 * 게임 내에서 메시지를 브로드캐스트하고 구독하는 기능을 제공
 */
UCLASS()
class EMBERUTILS_API UMessageBus : public UObject
{
	GENERATED_BODY()
	
public:
	static UMessageBus* GetInstance();

public:
	/** 브로드캐스트 */
	void BroadcastMessage(const FName MessageType, UObject* Payload) const;
	/** 구독 */
	void Subscribe(const FName MessageType, const FMessageDelegate& Delegate);
	/** 구독 해제 */
	void Unsubscribe(const FName MessageType, const FMessageDelegate& Delegate);

private:
	UMessageBus();
	~UMessageBus();

	mutable FCriticalSection Mutex;
	TMap<FName, TArray<FMessageDelegate>> Subscribers;
};
