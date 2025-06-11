// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/WorldInteractable/MiniGame/MinigameInteractableActor.h"

void AMiniGameInteractableActor::Interact_Implementation(AActor* Interactor)
{
	// 중복 실행 방지 코드 필요
	Super::Interact_Implementation(Interactor);
	// Interactor 저장 필요
	BeginMiniGame();
}

void AMiniGameInteractableActor::BeginMiniGame()
{
	for (UMiniGameFragment* Fragment : MiniGameFragments)
	{
		if (Fragment)
		{
			Fragment->OnMiniGameStart(this);
		}
	}
}

void AMiniGameInteractableActor::EndMiniGame()
{
	// 클리어 유무 확인 및 보상 지급
	
	for (UMiniGameFragment* Fragment : MiniGameFragments)
	{
		if (Fragment)
		{
			Fragment->OnMiniGameEnd();
		}
	}
}
