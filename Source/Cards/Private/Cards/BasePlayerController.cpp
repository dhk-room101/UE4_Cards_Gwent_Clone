// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"

#include "CardManager.h"
#include "CardLeaderInstance.h"
#include "CardSlot.h"

#include "CardsGameFlowController.h"
#include "CardsGameMain.h"
#include "CardsCardHolder.h"

#include "FiniteStateMachine.h"

#include "ldf.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

UBasePlayerController::UBasePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	_stateMachine = NewObject<UFiniteStateMachine>();
}

UFiniteStateMachine* UBasePlayerController::GetFiniteStateMachine()
{
	return _stateMachine;
}

void UBasePlayerController::ResetCurrentRoundStatus()
{
	if (GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer).Num() > 0)
	{
		SetCurrentRoundStatus(ROUND_PLAYER_STATUS_ACTIVE);
		bTurnOver = true;
		bHandPlayed = false;
	}
}

void UBasePlayerController::StartTurn()
{
	if (GetCurrentRoundStatus() == ROUND_PLAYER_STATUS_DONE)
	{
		return;
	}

	bTurnOver = false;
}

void UBasePlayerController::SkipTurn()
{
	SetCurrentRoundStatus(ROUND_PLAYER_STATUS_DONE);

	if (GetBoardRenderer()->transactionCardSlot)
	{
		DeclineCardTransaction();
	}

	GetGameFlowController()->playerControllers[nPlayer]->bInputEnabled = false;

	bTurnOver = true;

	_stateMachine->ChangeState(STATE_IDLE);
}

void UBasePlayerController::State_begin_ApplyingCard()
{
	LogWarning("[[BASE]] State_begin_ApplyingCard");
	UCardInstance* cardInstance = _decidedCardTransaction->sourceCardInstanceRef;

	if (cardInstance->IsA(UCardLeaderInstance::StaticClass()))
	{
		Cast<UCardLeaderInstance>(cardInstance)->ApplyLeaderAbility(bIsAI);

		//ugly manual Leader reset for Player
		if (!bIsAI)
		{
			for (int32 i = 0; i < GetBoardRenderer()->P1Leader->GetChildrenCount(); i++)
			{
				if (GetBoardRenderer()->P1Leader->GetChildAt(i) && GetBoardRenderer()->P1Leader->GetChildAt(i)->IsA(UCardSlot::StaticClass()))
				{
					UCardSlot* _cardSlot = Cast<UCardSlot>(GetBoardRenderer()->P1Leader->GetChildAt(i));
					GetBoardRenderer()->P1Leader->RemoveChild(_cardSlot);
				}
			}

			UCardLeaderInstance* leaderP1 = Cast<UCardLeaderInstance>(GetCardManager()->SpawnCardInstance(GetCardManager()->playerDeckDefinitions[PLAYER_1]->nSelectedKingIndex, PLAYER_1));
			GetCardManager()->AddCardInstanceToList(leaderP1, CARD_LIST_LOC_LEADER, PLAYER_1);
		}

		//TODO visual and reset 
		_decidedCardTransaction = nullptr;
	}
	else if (_decidedCardTransaction->targetSlotID != CARD_LIST_LOC_INVALID)
	{
		TransferTransactionCardToDestination(_decidedCardTransaction->targetSlotID, _decidedCardTransaction->targetPlayerID);
	}
	else if (_decidedCardTransaction->targetCardInstanceRef)
	{
		ApplyTransactionCardToCardInstance(_decidedCardTransaction->targetCardInstanceRef);
	}
	else if (_decidedCardTransaction->sourceCardInstanceRef->templateRef->IsType(CardType_Global_Effect))
	{
		ApplyGlobalEffectTransactionCard();
	}
	else
	{
		DeclineCardTransaction();
	}

	GetBoardRenderer()->ActivateHoldersForCard(nullptr, true);

	GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().SetTimer(TimerHandleDelayAction, 1.2, false);
}

void UBasePlayerController::State_update_ApplyingCard()
{
	LogWarning("[[BASE]] State_update_ApplyingCard");
	if (!GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().IsTimerActive(TimerHandleDelayAction))
	{
		//TODO visual
		GetGameFlowController()->playerControllers[nPlayer]->bInputEnabled = false;

		//GetGameFlowController()->nCurrentPlayer = nOpponent;

		bTurnOver = true;

		_stateMachine->ChangeState(STATE_IDLE);
	}
}

void UBasePlayerController::TransferTransactionCardToDestination(int32 nCardListLocation, int32 nPlayerID)
{
	if (GetBoardRenderer()->transactionCardSlot)
	{
		GetCardManager()->AddCardInstanceIDToList(GetBoardRenderer()->transactionCardSlot->nInstanceId, nCardListLocation, nPlayerID);

		GetBoardRenderer()->ClearCardTransactionZoom();
	}
}

void UBasePlayerController::ApplyTransactionCardToCardInstance(UCardInstance* cardInstance)
{
	//TODO ApplyTransactionCardToCardInstance

	LogError("TODO ApplyTransactionCardToCardInstance");

}

void UBasePlayerController::ApplyGlobalEffectTransactionCard()
{
	//TODO ApplyGlobalEffectTransactionCard

	LogError("TODO ApplyGlobalEffectTransactionCard");

}

void UBasePlayerController::DeclineCardTransaction()
{
	GetBoardRenderer()->ClearCardTransactionZoom();
}
