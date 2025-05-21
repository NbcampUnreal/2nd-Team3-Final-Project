// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimal/EQC_Animal.h"

void UEQC_Animal::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	// 예시: 월드에서 "타겟이 될 수 있는 액터들"을 찾는다 (ex: 플레이어)
	// UWorld* World = QueryInstance.World;
	// if (!World) return;
	//
	// TArray<AActor*> OutActors;
	// UGameplayStatics::GetAllActorsOfClass(World, AYourTargetActorClass::StaticClass(), OutActors); //실제 게임에서 타겟이 될 수 있는 액터 클래스 (예: ACharacter, AAnimalBase 등)로 바꿔주세요.
	//
	// if (OutActors.Num() > 0)
	// {
	// 	UEnvQueryItemType_Actor::SetContextHelper(ContextData, OutActors);
	// }
}
