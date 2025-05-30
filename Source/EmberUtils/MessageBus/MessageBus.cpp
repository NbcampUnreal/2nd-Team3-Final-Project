#include "MessageBus.h"

UMessageBus::UMessageBus()
{
	
}

UMessageBus::~UMessageBus()
{
	
}

UMessageBus* UMessageBus::GetInstance()
{
	static UMessageBus* Instance = nullptr;
	if (!Instance)
	{
		Instance = NewObject<UMessageBus>();
		Instance->AddToRoot();
	}
	return Instance;
}

void UMessageBus::BroadcastMessage(const FName MessageType, UObject* Payload) const
{
	TArray<FMessageDelegate> DelegatesCopy;
	{
		FScopeLock Lock(&Mutex);
		if (const TArray<FMessageDelegate>* Found = Subscribers.Find(MessageType))
		{
			DelegatesCopy = *Found;
		}
	}

	for (const FMessageDelegate& Delegate : DelegatesCopy)
	{
		Delegate.ExecuteIfBound(MessageType, Payload);
	}
}

void UMessageBus::Subscribe(const FName MessageType, const FMessageDelegate& Delegate)
{
	FScopeLock Lock(&Mutex);
	Subscribers.FindOrAdd(MessageType).Add(Delegate);
}

void UMessageBus::Unsubscribe(const FName MessageType, const FMessageDelegate& Delegate)
{
	FScopeLock Lock(&Mutex);
	if (TArray<FMessageDelegate>* Found = Subscribers.Find(MessageType))
	{
		// 객체 단위로 Delegate 제거
		Found->RemoveAll([&](const FMessageDelegate& InDelegate)
		{
			return InDelegate.IsBoundToObject(Delegate.GetUObject());
		});
		
		if (Found->IsEmpty())
		{
			Subscribers.Remove(MessageType);
		}
	}
}
