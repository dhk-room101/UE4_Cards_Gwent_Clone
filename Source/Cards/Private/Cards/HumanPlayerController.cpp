// Fill out your copyright notice in the Description page of Project Settings.

#include "HumanPlayerController.h"

#include "FiniteStateMachine.h"

#include "CardLeaderInstance.h"
#include "CardInstance.h"
#include "CardManager.h"
#include "CardSlot.h"
#include "CardsGameMain.h"
#include "CardsCardHolder.h"
#include "CardsGameFlowController.h"

#include "UMG.h"
#include "CardDescription.h"

UHumanPlayerController::UHumanPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	_stateMachine->ownerRef = this;

	UFunction* fstate_begin_Idle = FindFunction("State_begin_Idle");
	UFunction* fstate_about_to_update = FindFunction("State_about_to_update");
	UFunction* fstate_leave_Idle = FindFunction("State_leave_Idle");
	_stateMachine->AddState(STATE_IDLE, fstate_begin_Idle, fstate_about_to_update, fstate_leave_Idle);

	UFunction* fstate_begin_ChoosingMove = FindFunction("State_begin_ChoosingMove");
	UFunction* fstate_update_ChoosingMove = FindFunction("State_update_ChoosingMove");
	UFunction* fstate_leave_ChoosingMove = FindFunction("State_leave_ChoosingMove");
	_stateMachine->AddState(STATE_CHOOSINGMOVE, fstate_begin_ChoosingMove, fstate_update_ChoosingMove, fstate_leave_ChoosingMove);

	UFunction* fstate_begin_ChoosingHandler = FindFunction("State_begin_ChoosingHandler");
	UFunction* fstate_update_ChoosingHandler = FindFunction("State_update_ChoosingHandler");
	_stateMachine->AddState(STATE_CHOOSINGHANDLER, fstate_begin_ChoosingHandler, fstate_update_ChoosingHandler, nullptr);

	UFunction* fstate_begin_ChoosingTargetCard = FindFunction("State_begin_ChoosingTargetCard");
	UFunction* fstate_update_ChoosingTargetCard = FindFunction("State_update_ChoosingTargetCard");
	_stateMachine->AddState(STATE_CHOOSINGTARGETCARD, fstate_begin_ChoosingTargetCard, fstate_update_ChoosingTargetCard, nullptr);

	UFunction* fstate_begin_WaitConfirmation = FindFunction("State_begin_WaitConfirmation");
	UFunction* fstate_update_WaitConfirmation = FindFunction("State_update_WaitConfirmation");
	_stateMachine->AddState(STATE_WAITCONFIRMATION, fstate_begin_WaitConfirmation, fstate_update_WaitConfirmation, nullptr);

	UFunction* fstate_begin_ApplyingCard = FindFunction("State_begin_ApplyingCard");
	UFunction* fstate_update_ApplyingCard = FindFunction("State_update_ApplyingCard");
	_stateMachine->AddState(STATE_APPLYINGCARD, fstate_begin_ApplyingCard, fstate_update_ApplyingCard, nullptr);
}

void UHumanPlayerController::StartTurn()
{
	if (GetCurrentRoundStatus() == ROUND_PLAYER_STATUS_DONE)
	{
		return;
	}

	if (GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer).Num() == 0 &&
		GetCardManager()->GetCardLeader(nPlayer) != nullptr &&
		!GetCardManager()->GetCardLeader(nPlayer)->bCanBeUsed)
	{
		//SetCurrentRoundStatus(ROUND_PLAYER_STATUS_DONE);
		SkipTurn();
	}
	else
	{
		Super::StartTurn();
		_stateMachine->ChangeState(STATE_CHOOSINGMOVE);
	}
}

void UHumanPlayerController::SkipTurn()
{
	Super::SkipTurn();

	State_about_to_update();
	bPlayerTurnMessageSeen = false;
}

void UHumanPlayerController::HandleHolderChosen(UCardsCardHolder* cardHolder)
{
	UCardSlot* currentCardSlot = GetBoardRenderer()->transactionCardSlot;
	UCardInstance* transactionInstance = nullptr;

	LogWarning("GFX handleHolderChosen <"
		+ ConvertStateToString(_stateMachine->GetCurrentState())
		+ "> " + currentCardSlot->CardDescription->TextCardTitle->GetText().ToString()
		+ " | " + cardHolder->sTag);

	if (currentCardSlot && _stateMachine->GetCurrentState() == STATE_CHOOSINGHANDLER)
	{
		transactionInstance = GetCardManager()->GetCardInstance(currentCardSlot->nInstanceId);

		_decidedCardTransaction = NewObject<UCardTransaction>();

		_decidedCardTransaction->targetPlayerID = cardHolder->nPlayer;
		_decidedCardTransaction->targetSlotID = cardHolder->nCardHolder;
		_decidedCardTransaction->targetCardInstanceRef = nullptr;
		_decidedCardTransaction->sourceCardInstanceRef = transactionInstance;

		_stateMachine->ChangeState(STATE_APPLYINGCARD);
	}
}

void UHumanPlayerController::State_about_to_update()
{
	//DHK
	//LogWarning("[[PLAYER]] State_about_to_update");
}

void UHumanPlayerController::State_begin_Idle()
{
	LogWarning("[[PLAYER]] State_begin_Idle");
	//TODO visual
	_decidedCardTransaction = nullptr;
	DeclineCardTransaction();
}

void UHumanPlayerController::State_leave_Idle()
{
	//TODO visual
	LogWarning("[[PLAYER]] State_leave_Idle");
}

void UHumanPlayerController::State_begin_ChoosingMove()
{
	LogWarning("[[PLAYER]] State_begin_ChoosingMove");
	//TODO visual
	if (!bPlayerTurnMessageSeen)
	{
		bPlayerTurnMessageSeen = true;

		bHandPlayed = true;
	}
}

void UHumanPlayerController::State_update_ChoosingMove()
{
	LogWarning("[[PLAYER]] State_update_ChoosingMove");
	GetGameFlowController()->playerControllers[nPlayer]->bInputEnabled = true;

	GetBoardRenderer()->HandleSkipTurnButton(false);

	UCardInstance* cardInstance = nullptr;

	bool bIsLeader = false;
	bool bHasEffectUnsummonDummy = false;
	bool bIsGlobalEffect = false;

	State_about_to_update();

	if (GetBoardRenderer()->transactionCardSlot)
	{
		cardInstance = GetCardManager()->GetCardInstance(GetBoardRenderer()->transactionCardSlot->nInstanceId);

		if (!cardInstance)
		{
			LogError("GFX [ERROR] - 'State_update_ChoosingMove' cardInstance null for playerIndex " + FString::FromInt(nPlayer));
			return;
		}

		bIsLeader = cardInstance->IsA(UCardLeaderInstance::StaticClass());
		bHasEffectUnsummonDummy = cardInstance->templateRef->HasEffect(CardEffect_UnsummonDummy);
		bIsGlobalEffect = cardInstance->templateRef->IsType(CardType_Global_Effect);

		if (bIsGlobalEffect) /*bIsLeader || *///DHK
		{
			_stateMachine->ChangeState(STATE_WAITCONFIRMATION);
		}
		else if (bHasEffectUnsummonDummy)
		{
			_stateMachine->ChangeState(STATE_CHOOSINGTARGETCARD);
		}
		else
		{
			_stateMachine->ChangeState(STATE_CHOOSINGHANDLER);
		}
	}
}

void UHumanPlayerController::State_leave_ChoosingMove()
{
	LogWarning("[[PLAYER]] State_leave_ChoosingMove");
	//TODO visual
	GetBoardRenderer()->HandleSkipTurnButton(true);
}

void UHumanPlayerController::State_begin_ChoosingHandler()
{
	LogWarning("[[PLAYER]] State_begin_ChoosingHandler");
	if (GetBoardRenderer() && GetBoardRenderer()->transactionCardSlot)
	{
		GetBoardRenderer()->ActivateHoldersForCard(GetCardManager()->GetCardInstance(GetBoardRenderer()->transactionCardSlot->nInstanceId), false);
	}
}

void UHumanPlayerController::State_update_ChoosingHandler()
{
	LogWarning("[[PLAYER]] State_update_ChoosingHandler");
	State_about_to_update();
}

void UHumanPlayerController::State_begin_ChoosingTargetCard()
{
	//TODO visual
	LogWarning("[[PLAYER]] State_begin_ChoosingTargetCard");
}

void UHumanPlayerController::State_update_ChoosingTargetCard()
{
	LogWarning("[[PLAYER]] State_update_ChoosingTargetCard");
	State_about_to_update();
}

void UHumanPlayerController::State_begin_WaitConfirmation()
{
	LogWarning("[[PLAYER]] State_begin_WaitConfirmation");
	//_cardConfirmation = false;
	//TODO visual
}

void UHumanPlayerController::State_update_WaitConfirmation()
{
	LogWarning("[[PLAYER]] State_update_WaitConfirmation");
	State_about_to_update();
	//TODO finish State_update_WaitConfirmation
// 	if (_cardConfirmation && _transactionCard)
// 	{
// 		_cardConfirmation = false;
// 		_decidedCardTransaction = NewObject<UCardTransaction>();
// 		_decidedCardTransaction->targetPlayerID = nPlayer;
// 		_decidedCardTransaction->sourceCardInstanceRef = GetCardManager()->GetCardInstance(_transactionCard->instanceId);
// 		_stateMachine->ChangeState(STATE_APPLYINGCARD);
// 	}
}

void UHumanPlayerController::State_begin_ApplyingCard()
{
	Super::State_begin_ApplyingCard();

	LogWarning("[[PLAYER]] State_begin_ApplyingCard");
	GetBoardRenderer()->ActivateHoldersForCard(nullptr, true);
	//TODO visual
}

void UHumanPlayerController::State_update_ApplyingCard()
{
	LogWarning("[[PLAYER]] State_update_ApplyingCard");
	if (!GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().IsTimerActive(TimerHandleDelayAction))
	{
		State_about_to_update();

		GetGameFlowController()->playerControllers[nPlayer]->bInputEnabled = false;

		bTurnOver = true;
		bPlayerTurnMessageSeen = false;

		_stateMachine->ChangeState(STATE_IDLE);
	}
}
