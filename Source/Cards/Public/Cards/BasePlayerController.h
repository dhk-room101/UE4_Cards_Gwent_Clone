// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "BasePlayerController.generated.h"

class UFiniteStateMachine;
class UCardsGameFlowController;
class UCardTransaction;
class UCardInstance;
class UCardSlot;

/**
*
*/
UCLASS()
class CARDS_API UBasePlayerController : public UObject
{
	GENERATED_BODY()

public:
	UBasePlayerController(const FObjectInitializer& ObjectInitializer);

	UFiniteStateMachine* GetFiniteStateMachine();

	void ResetCurrentRoundStatus();

	virtual void StartTurn();
	virtual void SkipTurn();

	int32 nOpponent;
	int32 nPlayer;

	bool bCardZoomEnabled = false;
	bool bInputEnabled = false;
	bool bTurnOver = true;
	bool bHandPlayed = false;

	FORCEINLINE int32 GetCurrentRoundStatus() const { return nCurrentRoundStatus; }
	FORCEINLINE void SetCurrentRoundStatus(int32 nStatus) { nCurrentRoundStatus = nStatus; }

private:
	int32 nCurrentRoundStatus;

protected:

	UFUNCTION()
		virtual void State_begin_ApplyingCard();
	UFUNCTION()
		virtual void State_update_ApplyingCard();

	void TransferTransactionCardToDestination(int32 nCardListLocation, int32 nPlayerID);
	void ApplyTransactionCardToCardInstance(UCardInstance* cardInstance);
	void ApplyGlobalEffectTransactionCard();
	void DeclineCardTransaction();

	UPROPERTY()
		bool bIsAI = false;

	UPROPERTY()
		UCardTransaction* _decidedCardTransaction = nullptr;

	UPROPERTY()
		UFiniteStateMachine* _stateMachine;

	UPROPERTY()
		FTimerHandle TimerHandleDelayAction;
};
