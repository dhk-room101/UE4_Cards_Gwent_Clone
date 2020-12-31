// Fill out your copyright notice in the Description page of Project Settings.

#include "AIPlayerController.h"

#include "FiniteStateMachine.h"

#include "CardManager.h"
#include "CardTemplate.h"
#include "CardLeaderInstance.h"

#include "CardsGameFlowController.h"
#include "CardsGameMain.h"
#include "CardsCardHolder.h"
#include "CardSlot.h"

#include "ldf.h"

#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

UAIPlayerController::UAIPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	_stateMachine->ownerRef = this;

	bIsAI = true;

	UFunction* fstate_begin_Idle = FindFunction("State_begin_Idle");
	UFunction* fstate_leave_Idle = FindFunction("State_leave_Idle");
	_stateMachine->AddState(STATE_IDLE, fstate_begin_Idle, nullptr, fstate_leave_Idle);

	UFunction* fstate_begin_ChoosingMove = FindFunction("State_begin_ChoosingMove");
	UFunction* fstate_update_ChoosingMove = FindFunction("State_update_ChoosingMove");
	_stateMachine->AddState(STATE_CHOOSINGMOVE, fstate_begin_ChoosingMove, fstate_update_ChoosingMove, nullptr);

	UFunction* fstate_begin_SendingCardToTransaction = FindFunction("State_begin_SendingCardToTransaction");
	UFunction* fstate_update_SendingCardToTransaction = FindFunction("State_update_SendingCardToTransaction");
	_stateMachine->AddState(STATE_SENDINGCARDTOTRANSACTION, fstate_begin_SendingCardToTransaction, fstate_update_SendingCardToTransaction, nullptr);

	UFunction* fstate_begin_DelayBetweenActions = FindFunction("State_begin_DelayBetweenActions");
	UFunction* fstate_update_DelayBetweenActions = FindFunction("State_update_DelayBetweenActions");
	_stateMachine->AddState(STATE_DELAYBETWEENACTIONS, fstate_begin_DelayBetweenActions, fstate_update_DelayBetweenActions, nullptr);

	UFunction* fstate_begin_ApplyingCard = FindFunction("State_begin_ApplyingCard");
	UFunction* fstate_update_ApplyingCard = FindFunction("State_update_ApplyingCard");
	_stateMachine->AddState(STATE_APPLYINGCARD, fstate_begin_ApplyingCard, fstate_update_ApplyingCard, nullptr);
}

void UAIPlayerController::StartTurn()
{
	if (GetCurrentRoundStatus() == ROUND_PLAYER_STATUS_DONE)
	{
		return;
	}

	if (GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer).Num() == 0 &&
		GetCardManager()->GetCardLeader(nPlayer) != nullptr &&
		!GetCardManager()->GetCardLeader(nPlayer)->bCanBeUsed)
	{
		SkipTurn();
	}
	else
	{
		Super::StartTurn();
		_stateMachine->ChangeState(STATE_CHOOSINGMOVE);
	}
}

void UAIPlayerController::SkipTurn()
{
	Super::SkipTurn();
}

void UAIPlayerController::State_begin_Idle()
{
	LogWarning("[[AI]] State_begin_Idle");
	if (nAttitude == TACTIC_PASS)
	{
		SetCurrentRoundStatus(ROUND_PLAYER_STATUS_DONE);
	}

	bTurnOver = true;

	if (GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer).Num() == 0 &&
		GetCardManager()->GetCardLeader(nPlayer) != nullptr &&
		!GetCardManager()->GetCardLeader(nPlayer)->bCanBeUsed)
	{
		SetCurrentRoundStatus(ROUND_PLAYER_STATUS_DONE);
	}

	/*if (GetBoardRenderer())
	{
		//GetBoardRenderer()->GetCardHolder(CARD_LIST_LOC_LEADER, nPlayer)->DisableLeaderUse(false);
		//GetBoardRenderer()->DisableLeaderUse(nPlayer);
	}*/

	//TODO update visual
}

void UAIPlayerController::State_leave_Idle()
{
	LogWarning("[[AI]] State_leave_Idle");
	//TODO update visual
}

void UAIPlayerController::State_begin_ChoosingMove()
{
	LogWarning("[[AI]] State_begin_ChoosingMove");
	bHandPlayed = true;

	GetCardManager()->CalculateCardPowerPotentials();
	ChooseAttitude();


	LogWarning("GFX -#AI# ai has decided to use the following attitude:" + ConvertAttitudeToString(nAttitude));



	_decidedCardTransaction = DecideWhichCardToPlay();

	if (_decidedCardTransaction == nullptr && nAttitude != TACTIC_PASS)
	{
		nAttitude = TACTIC_PASS;
	}
	else if (bCurrentRoundCritical && _decidedCardTransaction != nullptr
		&& !_decidedCardTransaction->sourceCardInstanceRef->templateRef->HasEffect(CardEffect_UnsummonDummy)
		&& _decidedCardTransaction->powerChangeResult < 0 && GetCardManager()->GetAllCreaturesInHand(nPlayer).Num() == 0)
	{
		_decidedCardTransaction = nullptr;
		nAttitude = TACTIC_PASS;
	}


	if (_decidedCardTransaction)
		LogWarning("GFX -#AI# the ai decided on the following transaction: " + _decidedCardTransaction->GetInfo());


}

void UAIPlayerController::State_update_ChoosingMove()
{
	LogWarning("[[AI]] State_update_ChoosingMove");
	if (nAttitude == TACTIC_PASS || _decidedCardTransaction == nullptr)
	{
		_stateMachine->ChangeState(STATE_IDLE);
		if (nAttitude != TACTIC_PASS)
		{
			LogWarning("GFX -#AI#--------------- WARNING ---------- AI is passing since chosen tactic was unable to find a transaction it liked");
		}
		nAttitude = TACTIC_PASS;
	}
	else
	{
		GetGameFlowController()->playerControllers[nPlayer]->bInputEnabled = true;

		_stateMachine->ChangeState(STATE_SENDINGCARDTOTRANSACTION);
	}
}

void UAIPlayerController::State_begin_SendingCardToTransaction()
{
	LogWarning("[[AI]] State_begin_SendingCardToTransaction");


	LogWarning("GFX -#AI# AI is sending the following card into transaction: " + GetCardManager()->_cardTemplates[_decidedCardTransaction->sourceCardInstanceRef->nTemplateId]->sTitle);



	//TODO visual
	//StartCardTransaction(_decidedCardTransaction->sourceCardInstanceRef->nInstanceId);
}

void UAIPlayerController::State_update_SendingCardToTransaction()
{
	LogWarning("[[AI]] State_update_SendingCardToTransaction");
	_stateMachine->ChangeState(STATE_DELAYBETWEENACTIONS);
}

void UAIPlayerController::State_begin_DelayBetweenActions()
{
	LogWarning("[[AI]] State_begin_DelayBetweenActions");
	GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().SetTimer(TimerHandleDelayAction, 1.2, false);

	if (GetGameFlowController()->playerControllers[nPlayer]->bInputEnabled)
	{
		if (GetBoardRenderer()->ZoomHolder->GetChildrenCount() == 0)
		{
			GetBoardRenderer()->PlaySound("DealCard");

			GetBoardRenderer()->SpawnCardInstance(_decidedCardTransaction->sourceCardInstanceRef, CARD_LIST_LOC_ZOOM, nPlayer);
			GetBoardRenderer()->HandleTransactionButtons(true);

			if (GetBoardRenderer() && GetBoardRenderer()->transactionCardSlot)
			{
				GetBoardRenderer()->ActivateHoldersForCard(GetCardManager()->GetCardInstance(GetBoardRenderer()->transactionCardSlot->nInstanceId), false);
			}
		}
	}
}

void UAIPlayerController::State_update_DelayBetweenActions()
{
	LogWarning("[[AI]] State_update_DelayBetweenActions");
	if (!GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().IsTimerActive(TimerHandleDelayAction))
		_stateMachine->ChangeState(STATE_APPLYINGCARD);
}

UCardTransaction* UAIPlayerController::DecideWhichCardToPlay()
{
	TArray<UCardInstance*> cardsList;

	UCardTransaction* decidedTransaction = nullptr;
	UCardInstance* firstCardInHandWithEffect = nullptr;
	UCardInstance* cardHighestCompareTo = nullptr;

	int32 nCounter = 0;
	int32 nBetterAverage = 0;
	int32 nMinMax = 0;
	int32 nPlayerScore = GetCardManager()->currentPlayerScores[nPlayer];
	int32 nOpponentScore = GetCardManager()->currentPlayerScores[nOpponent];
	int32 nScoreDifference = nPlayerScore - nOpponentScore;

	switch (nAttitude)
	{
	case TACTIC_SPY_DUMMY_BEST_THEN_PASS:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_SPY_DUMMY_BEST_THEN_PASS");
		firstCardInHandWithEffect = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_Draw2, nPlayer);

		if (firstCardInHandWithEffect)
		{
			return firstCardInHandWithEffect->GetOptimalTransaction();
		}

		firstCardInHandWithEffect = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_UnsummonDummy, nPlayer);

		if (firstCardInHandWithEffect)
		{
			cardHighestCompareTo = GetCardManager()->GetHigherOrLowerValueTargetCardOnBoard(firstCardInHandWithEffect, nPlayer, true, true);

			if (cardHighestCompareTo)
			{
				decidedTransaction = firstCardInHandWithEffect->GetOptimalTransaction();
				decidedTransaction->targetCardInstanceRef = cardHighestCompareTo;

				return decidedTransaction;
			}
		}

		nAttitude = TACTIC_PASS;

		break;
	}
	case TACTIC_MINIMIZE_LOSS:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_MINIMIZE_LOSS");
		firstCardInHandWithEffect = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_UnsummonDummy, nPlayer);

		if (firstCardInHandWithEffect)
		{
			cardHighestCompareTo = GetHighestValueCardOnBoard();

			if (cardHighestCompareTo)
			{
				decidedTransaction = firstCardInHandWithEffect->GetOptimalTransaction();
				decidedTransaction->targetCardInstanceRef = cardHighestCompareTo;

				return decidedTransaction;
			}
		}

		firstCardInHandWithEffect = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_Draw2, nPlayer);

		if (firstCardInHandWithEffect != nullptr)
		{
			return firstCardInHandWithEffect->GetOptimalTransaction();
		}

		nAttitude = TACTIC_PASS;

		break;
	}
	case TACTIC_MINIMIZE_WIN:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_MINIMIZE_WIN");
		firstCardInHandWithEffect = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_UnsummonDummy, nPlayer);

		if (firstCardInHandWithEffect)
		{
			cardHighestCompareTo = GetHighestValueCardOnBoardWithEffectLessThan(nScoreDifference);

			if (cardHighestCompareTo)
			{
				decidedTransaction = firstCardInHandWithEffect->GetOptimalTransaction();

				if (decidedTransaction)
				{
					decidedTransaction->targetCardInstanceRef = cardHighestCompareTo;

					return decidedTransaction;
				}
			}
		}

		cardsList = GetCardManager()->GetCardsInHandWithEffect(CardEffect_Draw2, nPlayer);

		nCounter = 0;
		while (nCounter < cardsList.Num())
		{
			firstCardInHandWithEffect = cardsList[nCounter];
			decidedTransaction = firstCardInHandWithEffect->GetOptimalTransaction();

			if (firstCardInHandWithEffect && FMath::Abs(decidedTransaction->powerChangeResult) < FMath::Abs(nScoreDifference))
			{
				return firstCardInHandWithEffect->GetOptimalTransaction();
			}

			nCounter++;
		}

		nAttitude = TACTIC_PASS;

		break;
	}
	case TACTIC_MAXIMIZE_WIN:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_MAXIMIZE_WIN");
		cardsList = GetCardsBasedOnCriteria(SortType_PowerChange);

		if (cardsList.Num() > 0)
		{
			cardHighestCompareTo = cardsList[(cardsList.Num() - 1)];
			if (cardHighestCompareTo)
			{
				return cardHighestCompareTo->GetOptimalTransaction();
			}
		}

		break;
	}
	case TACTIC_AVERAGE_WIN:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_AVERAGE_WIN");
		cardsList = GetCardsBasedOnCriteria(SortType_PowerChange);

		nBetterAverage = -1;

		while (nCounter < cardsList.Num() && nBetterAverage == -1)
		{
			firstCardInHandWithEffect = cardsList[nCounter];
			decidedTransaction = firstCardInHandWithEffect->GetOptimalTransaction();

			if (decidedTransaction->powerChangeResult > FMath::Abs(nScoreDifference))
			{
				nBetterAverage = nCounter;
			}

			nCounter++;
		}

		if (nBetterAverage != -1)
		{
			nMinMax = FMath::Min(nBetterAverage, FMath::Max((cardsList.Num() - 1), nBetterAverage + FPlatformMath::FloorToInt(UKismetMathLibrary::RandomFloatInRange(0, 1) * ((cardsList.Num() - 1) - nBetterAverage))));

			cardHighestCompareTo = cardsList[nMinMax];

			if (cardHighestCompareTo)
			{
				return cardHighestCompareTo->GetOptimalTransaction();
			}
		}
		else if (cardsList.Num() > 0)
		{
			cardHighestCompareTo = cardsList[(cardsList.Num() - 1)];

			if (cardHighestCompareTo)
			{
				return cardHighestCompareTo->GetOptimalTransaction();
			}
		}

		break;
	}
	case TACTIC_MINIMAL_WIN:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_MINIMAL_WIN");
		cardsList = GetCardsBasedOnCriteria(SortType_PowerChange);

		nCounter = 0;
		while (nCounter < cardsList.Num())
		{
			firstCardInHandWithEffect = cardsList[nCounter];
			decidedTransaction = firstCardInHandWithEffect->GetOptimalTransaction();

			if (decidedTransaction->powerChangeResult > FMath::Abs(nScoreDifference))
			{
				cardHighestCompareTo = firstCardInHandWithEffect;

				break;
			}

			nCounter++;
		}

		if (!cardHighestCompareTo && cardsList.Num() > 0)
		{
			cardHighestCompareTo = cardsList[(cardsList.Num() - 1)];
		}

		if (cardHighestCompareTo)
		{
			return cardHighestCompareTo->GetOptimalTransaction();
		}

		break;
	}
	case TACTIC_JUST_WAIT:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_JUST_WAIT");
		cardsList = GetCardsBasedOnCriteria(SortType_StrategicValue);

		if (cardsList.Num() == 0)
		{
			return nullptr;
		}

		nCounter = 0;
		while (nCounter < cardsList.Num())
		{
			firstCardInHandWithEffect = cardsList[nCounter];
			decidedTransaction = firstCardInHandWithEffect->GetOptimalTransaction();

			if (decidedTransaction)
			{
				if (bCurrentRoundCritical)
				{
					if (decidedTransaction &&
						decidedTransaction->sourceCardInstanceRef->templateRef->IsType(CardType_Weather)
						&& (decidedTransaction->powerChangeResult < 0
							|| decidedTransaction->powerChangeResult < decidedTransaction->sourceCardInstanceRef->PotentialWeatherHarm()))
					{
						decidedTransaction = nullptr;
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			nCounter++;
		}

		return decidedTransaction;
	}
	case TACTIC_WAIT_DUMMY:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_WAIT_DUMMY");
		firstCardInHandWithEffect = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_UnsummonDummy, nPlayer);

		if (firstCardInHandWithEffect)
		{
			decidedTransaction = firstCardInHandWithEffect->GetOptimalTransaction();

			if (decidedTransaction->targetCardInstanceRef == nullptr)
			{
				decidedTransaction->targetCardInstanceRef = GetCardManager()->GetHigherOrLowerValueTargetCardOnBoard(firstCardInHandWithEffect, nPlayer, false);
			}

			if (decidedTransaction->targetCardInstanceRef != nullptr)
			{
				return decidedTransaction;
			}
		}

		LogWarning("GFX [ WARNING ] -#AI#---- Uh oh, was in TACTIC_WAIT_DUMMY but was unable to Get a valid dummy transaction :S");

		break;
	}
	case TACTIC_SPY:
	{
		LogWarning("GFX [ WARNING ] -#AI#---- TACTIC_SPY");
		firstCardInHandWithEffect = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_Draw2, nPlayer);

		if (firstCardInHandWithEffect != nullptr)
		{
			return firstCardInHandWithEffect->GetOptimalTransaction();
		}

		break;
	}
	default:
	{
		LogError("GFX [ WARNING ] -#AI#---- DEFAULT?!?");
		break;
	}
	}

	if (nAttitude != TACTIC_PASS && nAttitude != TACTIC_MINIMIZE_WIN)
	{
		firstCardInHandWithEffect = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_Draw2, nPlayer);

		if (firstCardInHandWithEffect)
		{
			return firstCardInHandWithEffect->GetOptimalTransaction();
		}
	}

	return nullptr;
}

UCardInstance* UAIPlayerController::GetHighestValueCardOnBoard()
{
	int32 nCounter = 0;
	UCardInstance* cardHighest = nullptr;
	UCardInstance* cardInstance = nullptr;

	TArray<UCardInstance*> cardsList;
	GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nPlayer, cardsList);
	GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nPlayer, cardsList);
	GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nPlayer, cardsList);

	while (nCounter < cardsList.Num())
	{
		cardInstance = cardsList[nCounter];

		if (cardHighest == nullptr
			|| cardInstance->templateRef->nPower + cardInstance->templateRef->GetBonusValue()
	> cardHighest->templateRef->nPower + cardHighest->templateRef->GetBonusValue())
		{
			cardHighest = cardInstance;
		}

		nCounter++;
	}

	return cardHighest;
}

UCardInstance* UAIPlayerController::GetHighestValueCardOnBoardWithEffectLessThan(int32 nDifference)
{
	int32 nCounter = 0;

	UCardInstance* cardInstance = nullptr;
	UCardInstance* cardLessThan = nullptr;

	TArray<UCardInstance*> cardsList;
	GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nPlayer, cardsList);
	GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nPlayer, cardsList);
	GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nPlayer, cardsList);

	while (nCounter < cardsList.Num())
	{
		cardInstance = cardsList[nCounter];

		if (!cardInstance->templateRef->HasEffect(CardEffect_SameTypeMorale)
			&& !cardInstance->templateRef->HasEffect(CardEffect_ImproveNeighbours)
			&& cardInstance->GetTotalPower() < nDifference
			&& (cardLessThan == nullptr || cardLessThan->templateRef->nPower + cardLessThan->templateRef->GetBonusValue() < cardInstance->templateRef->nPower + cardInstance->templateRef->GetBonusValue()))
		{
			cardLessThan = cardInstance;
		}

		nCounter++;
	}

	return cardLessThan;
}

TArray<UCardInstance*> UAIPlayerController::GetCardsBasedOnCriteria(int32 nSort)
{
	int32 nCounter = 0;

	UCardInstance* cardInstance = nullptr;
	UCardLeaderInstance* cardLeader = GetCardManager()->GetCardLeader(nPlayer);

	TArray<UCardInstance*> cardsBasedOnCriteriaList;
	TArray<UCardInstance*>& cardsHandList = GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer);

	if (cardLeader && cardLeader->bCanBeUsed)
	{
		cardLeader->RecalculatePowerPotential(GetCardManager());
		if (cardLeader->GetOptimalTransaction()->strategicValue != -1)
		{
			cardsBasedOnCriteriaList.Add(cardLeader);
		}
	}

	while (nCounter < cardsHandList.Num())
	{
		cardInstance = cardsHandList[nCounter];

		switch (nSort)
		{
		case SortType_None:
		{
			cardsBasedOnCriteriaList.Add(cardInstance);

			break;
		}
		case SortType_PowerChange:
		{
			if (cardInstance->GetOptimalTransaction()->powerChangeResult >= 0)
			{
				cardsBasedOnCriteriaList.Add(cardInstance);
			}

			break;
		}
		case SortType_StrategicValue:
		{
			if (cardInstance->GetOptimalTransaction()->strategicValue >= 0)
			{
				cardsBasedOnCriteriaList.Add(cardInstance);
			}

			break;
		}
		default:
		{
			break;
		}
		}

		nCounter++;
	}
	switch (nSort)
	{

		LogWarning("Doublecheck sorting AI");


	case SortType_StrategicValue:
	{
		cardsBasedOnCriteriaList.Sort(UAIPlayerController::StrategicValueSorter);

		break;
	}
	case SortType_PowerChange:
	{
		cardsBasedOnCriteriaList.Sort(UAIPlayerController::PowerChangeSorter);

		break;
	}
	default:
	{
		break;
	}
	}

	return cardsBasedOnCriteriaList;
}

void UAIPlayerController::ChooseAttitude()
{
	int32 nHandCounter = 0;
	int32 nRoundsCounter = 0;
	int32 nWinner = 0;

	float fMedian = 0.f;

	UCardInstance* dummyCardInstance = nullptr;

	TArray<UCardInstance*>& cardsHandList = GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer);
	if (cardsHandList.Num() == 0)
	{

		LogWarning("nAttitude chosen based on criteria 1");


		nAttitude = TACTIC_PASS;
		return;
	}

	bool bWinnerPlayer = false;
	bool bWinnerOpponent = false;

	int32 nCounterCreature = 0;
	int32 nCounterDummy = 0;
	int32 nCounterSpyPlayer = 0;

	while (nRoundsCounter < GetCardManager()->roundResults.Num())
	{
		if (GetCardManager()->roundResults[nRoundsCounter]->Played())
		{
			nWinner = GetCardManager()->roundResults[nRoundsCounter]->GetRoundWinningPlayer();

			if (nWinner == nPlayer || nWinner == PLAYER_INVALID)
			{
				bWinnerPlayer = true;
			}
			if (nWinner == nOpponent || nWinner == PLAYER_INVALID)
			{
				bWinnerOpponent = true;
			}
		}

		nRoundsCounter++;
	}

	bCurrentRoundCritical = bWinnerOpponent;

	nHandCounter = 0;
	while (nHandCounter < cardsHandList.Num())
	{
		if (cardsHandList[nHandCounter]->templateRef->IsType(CardType_Creature))
		{
			nCounterCreature++;
		}

		nHandCounter++;
	}

	int32 nHandCardsPlayer = cardsHandList.Num();
	int32 nHandCardsOpponent = GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nOpponent).Num();
	int32 nHandCardsDifference = nHandCardsPlayer - nHandCardsOpponent;
	int32 nCurrentScoreDifference = GetCardManager()->currentPlayerScores[nPlayer] - GetCardManager()->currentPlayerScores[nOpponent];
	int32 nOpponentRoundStatus = GetGameFlowController()->playerControllers[nOpponent]->GetCurrentRoundStatus();


	FString s;

	LogWarning("GFX -#AI# ###############################################################################");
	LogWarning("GFX -#AI#---------------------------- AI Deciding his next move --------------------------------");
	LogWarning("GFX -#AI#------ previousTactic: " + ConvertAttitudeToString(nAttitude));
	LogWarning("GFX -#AI#------ playerCardsInHand: " + FString::FromInt(nHandCardsPlayer));
	LogWarning("GFX -#AI#------ opponentCardsInHand: " + FString::FromInt(nHandCardsOpponent));
	LogWarning("GFX -#AI#------ cardAdvantage: " + FString::FromInt(nHandCardsDifference));
	LogWarning("GFX -#AI#------ scoreDifference: " + FString::FromInt(nCurrentScoreDifference) + ", his score: " + FString::FromInt(GetCardManager()->currentPlayerScores[nPlayer]) + ", enemy score: " + FString::FromInt(GetCardManager()->currentPlayerScores[nOpponent]));

	s = bWinnerOpponent ? "True" : "False";
	LogWarning("GFX -#AI#------ opponent has won: " + s);

	s = bWinnerPlayer ? "True" : "False";
	LogWarning("GFX -#AI#------ has won: " + s);

	LogWarning("GFX -#AI#------ Num units in hand: " + FString::FromInt(nCounterCreature));

	s = (GetGameFlowController()->playerControllers[nOpponent]->GetCurrentRoundStatus() == ROUND_PLAYER_STATUS_DONE) ? "True" : "False";
	LogWarning("GFX -#AI#------ has opponent passed: " + s);

	LogWarning("GFX =#AI#=======================================================================================");
	LogWarning("GFX -#AI#-----------------------------   AI CARDS AT HAND   ------------------------------------");

	nHandCounter = 0;

	while (nHandCounter < cardsHandList.Num())
	{
		LogWarning("GFX -#AI# Card Points[ "
			+ FString::FromInt(cardsHandList[nHandCounter]->templateRef->nPower)
			+ " ], Card: " + GetCardManager()->_cardTemplates[cardsHandList[nHandCounter]->nTemplateId]->sTitle);

		nHandCounter++;
	}
	LogWarning("GFX =#AI#=======================================================================================");



	int32 nFactionPlayer = GetCardManager()->playerDeckDefinitions[nPlayer]->GetDeckFaction();
	int32 nFactionOpponent = GetCardManager()->playerDeckDefinitions[nOpponent]->GetDeckFaction();
	int32 nCounterSpyOpponent = GetCardManager()->GetCardsInSlotIdWithEffect(CardEffect_Draw2, nOpponent).Num();

	if (nFactionPlayer == FactionId_Nilfgaard && nFactionOpponent != FactionId_Nilfgaard
		&& nOpponentRoundStatus == ROUND_PLAYER_STATUS_DONE && nCurrentScoreDifference == 0)
	{

		LogWarning("nAttitude chosen based on criteria 2");

		nAttitude = TACTIC_PASS;
	}
	else if (!bWinnerOpponent && nAttitude == TACTIC_SPY_DUMMY_BEST_THEN_PASS)
	{
		if (nOpponentRoundStatus != ROUND_PLAYER_STATUS_DONE)
		{

			LogWarning("nAttitude chosen based on criteria 3");

			nAttitude = TACTIC_SPY_DUMMY_BEST_THEN_PASS;
		}
	}
	else if (!bWinnerOpponent && GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_Draw2, nPlayer) != nullptr
		&& (UKismetMathLibrary::RandomFloatInRange(0, 1) < 0.2 || nCounterSpyOpponent > 1)
		&& nAttitude != TACTIC_SPY_DUMMY_BEST_THEN_PASS)
	{

		LogWarning("nAttitude chosen based on criteria 4");

		nAttitude = TACTIC_SPY;
	}
	else if (nAttitude == TACTIC_SPY
		&& GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_Draw2, nPlayer) != nullptr)
	{

		LogWarning("nAttitude chosen based on criteria 5");

		nAttitude = TACTIC_SPY;
	}
	else if (nOpponentRoundStatus != ROUND_PLAYER_STATUS_DONE)
	{
		if (nCurrentScoreDifference > 0)
		{
			if (bWinnerOpponent)
			{

				LogWarning("nAttitude chosen based on criteria 6");

				nAttitude = TACTIC_JUST_WAIT;
			}
			else
			{
				fMedian = nCounterCreature * nCounterCreature / 36;

				nAttitude = TACTIC_NONE;

				if (bWinnerPlayer)
				{
					nCounterDummy = GetCardManager()->GetCardsInHandWithEffect(CardEffect_UnsummonDummy, nPlayer).Num();
					nCounterSpyPlayer = GetCardManager()->GetCardsInHandWithEffect(CardEffect_Draw2, nPlayer).Num();

					if (UKismetMathLibrary::RandomFloatInRange(0, 1) < 0.2
						|| nHandCardsPlayer == nCounterDummy + nCounterSpyPlayer)
					{

						LogWarning("nAttitude chosen based on criteria 7");

						nAttitude = TACTIC_SPY_DUMMY_BEST_THEN_PASS;
					}
					else
					{
						dummyCardInstance = GetCardManager()->GetFirstCardInHandWithEffect(CardEffect_UnsummonDummy, nPlayer);

						if (dummyCardInstance != nullptr
							&& GetCardManager()->GetHigherOrLowerValueTargetCardOnBoard(dummyCardInstance, nPlayer, false) != nullptr)
						{

							LogWarning("nAttitude chosen based on criteria 8");

							nAttitude = TACTIC_WAIT_DUMMY;
						}
						else if (UKismetMathLibrary::RandomFloatInRange(0, 1) < nCurrentScoreDifference / 30
							&& UKismetMathLibrary::RandomFloatInRange(0, 1) < fMedian)
						{

							LogWarning("nAttitude chosen based on criteria 9");

							nAttitude = TACTIC_MAXIMIZE_WIN;
						}
					}
				}
				if (nAttitude == TACTIC_NONE)
				{
					if (UKismetMathLibrary::RandomFloatInRange(0, 1) < nHandCardsPlayer / 10 || nHandCardsPlayer > 8)
					{
						if (UKismetMathLibrary::RandomFloatInRange(0, 1) < 0.2
							|| nHandCardsPlayer == nCounterDummy + nCounterSpyPlayer)
						{

							LogWarning("nAttitude chosen based on criteria 10");

							nAttitude = TACTIC_SPY_DUMMY_BEST_THEN_PASS;
						}
						else
						{

							LogWarning("nAttitude chosen based on criteria 11");

							nAttitude = TACTIC_JUST_WAIT;
						}
					}
					else
					{

						LogWarning("nAttitude chosen based on criteria 12");

						nAttitude = TACTIC_PASS;
					}
				}
			}
		}
		else if (bWinnerPlayer)
		{
			nCounterDummy = GetCardManager()->GetCardsInHandWithEffect(CardEffect_UnsummonDummy, nPlayer).Num();
			nCounterSpyPlayer = GetCardManager()->GetCardsInHandWithEffect(CardEffect_Draw2, nPlayer).Num();

			if (!bWinnerOpponent && (UKismetMathLibrary::RandomFloatInRange(0, 1) < 0.2 || nHandCardsPlayer == nCounterDummy + nCounterSpyPlayer))
			{

				LogWarning("nAttitude chosen based on criteria 13");

				nAttitude = TACTIC_SPY_DUMMY_BEST_THEN_PASS;
			}
			else
			{

				LogWarning("nAttitude chosen based on criteria 14");

				nAttitude = TACTIC_MAXIMIZE_WIN;
			}
		}
		else if (bWinnerOpponent)
		{

			LogWarning("nAttitude chosen based on criteria 15");

			nAttitude = TACTIC_MINIMAL_WIN;
		}
		else if (!GetCardManager()->roundResults[0]->Played() && nCurrentScoreDifference < -11
			&& UKismetMathLibrary::RandomFloatInRange(0, 1) < (FMath::Abs(nCurrentScoreDifference) - 10) / 20)
		{
			if (UKismetMathLibrary::RandomFloatInRange(0, 1) < 0.9)
			{

				LogWarning("nAttitude chosen based on criteria 16");

				nAttitude = TACTIC_SPY_DUMMY_BEST_THEN_PASS;
			}
			else
			{

				LogWarning("nAttitude chosen based on criteria 17");


				nAttitude = TACTIC_PASS;
			}
		}
		else if (UKismetMathLibrary::RandomFloatInRange(0, 1) < nHandCardsPlayer / 10)
		{

			LogWarning("nAttitude chosen based on criteria 18");

			nAttitude = TACTIC_MINIMAL_WIN;
		}
		else if (UKismetMathLibrary::RandomFloatInRange(0, 1) < nHandCardsPlayer / 10)
		{

			LogWarning("nAttitude chosen based on criteria 19");

			nAttitude = TACTIC_AVERAGE_WIN;
		}
		else if (UKismetMathLibrary::RandomFloatInRange(0, 1) < nHandCardsPlayer / 10)
		{

			LogWarning("nAttitude chosen based on criteria 20");

			nAttitude = TACTIC_MAXIMIZE_WIN;
		}
		else if (nHandCardsPlayer <= 8 && UKismetMathLibrary::RandomFloatInRange(0, 1) > nHandCardsPlayer / 10)
		{

			LogWarning("nAttitude chosen based on criteria 21");

			nAttitude = TACTIC_PASS;
		}
		else
		{

			LogWarning("nAttitude chosen based on criteria 22");

			nAttitude = TACTIC_JUST_WAIT;
		}
	}
	else if (nAttitude != TACTIC_MINIMIZE_LOSS)
	{
		if (!bWinnerOpponent && nCurrentScoreDifference <= 0
			&& UKismetMathLibrary::RandomFloatInRange(0, 1) < nCurrentScoreDifference / 20)
		{

			LogWarning("nAttitude chosen based on criteria 23");

			nAttitude = TACTIC_MINIMIZE_LOSS;
		}
		else if (!bWinnerPlayer && nCurrentScoreDifference > 0)
		{

			LogWarning("nAttitude chosen based on criteria 24");

			nAttitude = TACTIC_MINIMIZE_WIN;
		}
		else if (nCurrentScoreDifference > 0)
		{

			LogWarning("nAttitude chosen based on criteria 25");

			nAttitude = TACTIC_PASS;
		}
		else
		{

			LogWarning("nAttitude chosen based on criteria 26");

			nAttitude = TACTIC_MINIMAL_WIN;
		}
	}
	else
	{

		LogWarning("nAttitude chosen based on criteria 27");

		nAttitude = TACTIC_MINIMIZE_LOSS;
	}
}
