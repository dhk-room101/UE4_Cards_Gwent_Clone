// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

#include "BasePlayerController.h"
#include "Engine/EngineTypes.h"

#include "CardsGameFlowController.generated.h"

class UFiniteStateMachine;

/**
 *
 */
UCLASS()
class CARDS_API UCardsGameFlowController : public UObject
{
	GENERATED_BODY()

public:
	UCardsGameFlowController(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
		UFiniteStateMachine* stateMachine = nullptr;

	UPROPERTY()
		int32 nCurrentPlayer = -1;
	UPROPERTY()
		int32 nCurrentRound = 0;
	UPROPERTY()
		int32 nLastRoundWinner = -1;

	UPROPERTY()
		bool bAllNeutralInRound = true;
	UPROPERTY()
		bool bPlayedCreaturesInRound = false;
	UPROPERTY()
		bool bPlayedThreeHeroesOneRound = false;
	UPROPERTY()
		bool bRoundStart = false;

	TArray<UBasePlayerController*> playerControllers;

protected:
	UPROPERTY()
		bool bSawRoundEndTutorial = false;
	UPROPERTY()
		bool bSawScoreChangeTutorial = false;
	UPROPERTY()
		bool bSawRoundStartTutorial = false;
	UPROPERTY()
		bool bSawEndGameTutorial = false;
	UPROPERTY()
		bool bSawStartMessage = false;//private?

	UFUNCTION()
		void State_update_Initializing();
	UFUNCTION()
		void State_leave_Initializing();

	UFUNCTION()
		void State_begin_Tutorials();
	UFUNCTION()
		void State_update_Tutorials();

	UFUNCTION()
		void State_begin_SpawnLeaders();
	UFUNCTION()
		void State_update_SpawnLeaders();

	UFUNCTION()
		void State_begin_CoinToss();
	UFUNCTION()
		void State_update_CoinToss();

	UFUNCTION()
		void State_begin_Mulligan();
	UFUNCTION()
		void State_update_Mulligan();

	UFUNCTION()
		void State_begin_RoundStart();
	UFUNCTION()
		void State_update_RoundStart();

	UFUNCTION()
		void State_begin_PlayerTurn();
	UFUNCTION()
		void State_update_PlayerTurn();
	UFUNCTION()
		void State_leave_PlayerTurn();

	UFUNCTION()
		void State_begin_ChangingPlayer();
	UFUNCTION()
		void State_update_ChangingPlayer();

	UFUNCTION()
		void State_begin_ShowingRoundResult();
	UFUNCTION()
		void State_update_ShowingRoundResult();

	UFUNCTION()
		void State_begin_ClearingBoard();
	UFUNCTION()
		void State_update_ClearingBoard();
	UFUNCTION()
		void State_leave_ClearingBoard();

	UFUNCTION()
		void State_begin_ShowingFinalResult();
	UFUNCTION()
		void State_update_ShowingFinalResult();

	UFUNCTION()
		void State_begin_Reset();

	UPROPERTY()
		FTimerHandle TimerHandleDelayAction;

	void QueueMessage(int32 nState, int32 nPlayer);
	bool IsShowingMessage();

private:
	UPROPERTY()
		int32 _nMulliganCardsCount = 0;
	UPROPERTY()
		bool _bMulliganDecided = false;

	void OnEndGameResult(int32 nWinner);
	void OnShowMessageEnd();

	TArray<int32> messageQueue;
};
