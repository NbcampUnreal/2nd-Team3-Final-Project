// Fill out your copyright notice in the Description page of Project Settings.


#include "Loop/LoopTimeOfDaySubsystem.h"

#include "GameLoopManagerSubsystem.h"

void ULoopTimeOfDaySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	TotalSec = EndTime - StartTime;
	TimeSpeed = (TotalSec) / static_cast<float>(RealTime);

	GameSecond = TimeSpeed;
	GameHour = 3600.f / TimeSpeed;
}

void ULoopTimeOfDaySubsystem::Deinitialize()
{
	Super::Deinitialize();
}

ULoopTimeOfDaySubsystem* ULoopTimeOfDaySubsystem::GetInstance(UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	return World->GetSubsystem<ULoopTimeOfDaySubsystem>();
}

void ULoopTimeOfDaySubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTime(DeltaTime);
}

void ULoopTimeOfDaySubsystem::StartTimeLoop()
{
	CurrentTime = 0.f;
	bEvent23Called = false;
	bEvent24Called = false;
	bPause = false;
}

float ULoopTimeOfDaySubsystem::GetNormalizedTime() const
{
	return FMath::Fmod(CurrentTime + StartTime, TotalSec) / TotalSec;
}

int ULoopTimeOfDaySubsystem::GetHour()
{
	return (CurrentTime + StartTime) / 3600;
}

void ULoopTimeOfDaySubsystem::SetPause(bool Val)
{
	bPause = Val;
	OnPauseChanged.Broadcast(Val);
}

bool ULoopTimeOfDaySubsystem::UpdateTime(float DeltaTime)
 {
	if (bPause) return true;

	CurrentTime += DeltaTime * TimeSpeed;
	float GameTime = CurrentTime + StartTime;
	
	/*UE_LOG(LogTemp, Warning, TEXT("Δt=%.3f | TimeSpeed=%.1f | Curr=%.1f | Game=%.1f | Hour=%.2f"), 
		DeltaTime, 
		TimeSpeed, 
		CurrentTime, 
		GameTime, 
		GameTime / 3600.f);*/
	
	if (!bEvent23Called && GameTime >= 23 * 3600.f)
	{
		bEvent23Called = true;
		//UE_LOG(LogTemp, Warning, TEXT(">>> 23:00 Event Triggered at Game=%.1f (Hour=%.2f)"), GameTime, GameTime / 3600.f);

		if (UGameLoopManagerSubsystem* LoopManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameLoopManagerSubsystem>())
		{
			LoopManagerSubsystem->EndLoopEvent();
		}
	}

	if (!bEvent24Called && GameTime >= 24 * 3600.f)
	{
		bEvent24Called = true;
		
		if (UGameLoopManagerSubsystem* LoopManagerSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UGameLoopManagerSubsystem>())
		{
			LoopManagerSubsystem->OnDayEnd24();
			SetPause(true);
		}
	}
	
	return true;
}