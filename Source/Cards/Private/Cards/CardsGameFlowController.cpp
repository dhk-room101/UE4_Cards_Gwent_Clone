// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsGameFlowController.h"

#include "FiniteStateMachine.h"

#include "HumanPlayerController.h"
#include "AIPlayerController.h"

#include "ldf.h"

#include "CardLeaderInstance.h"
#include "CardManager.h"
#include "CardEffectManager.h"
#include "CardsDeck.h"
#include "CardsGameMain.h"
#include "CardsGameInstance.h"
#include "GameResults.h"
#include "DeckBuilder.h"

#include "UMG.h"

#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

UCardsGameFlowController::UCardsGameFlowController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	stateMachine = NewObject<UFiniteStateMachine>();
	stateMachine->ownerRef = this;

	UFunction* fstate_update_Initializing = FindFunction("State_update_Initializing");
	UFunction* fstate_leave_Initializing = FindFunction("State_leave_Initializing");
	stateMachine->AddState(STATE_INITIALIZING, nullptr, fstate_update_Initializing, fstate_leave_Initializing);

	UFunction* fstate_begin_Tutorials = FindFunction("State_begin_Tutorials");
	UFunction* fstate_update_Tutorials = FindFunction("State_update_Tutorials");
	stateMachine->AddState(STATE_TUTORIALS, fstate_begin_Tutorials, fstate_update_Tutorials, nullptr);

	UFunction* fstate_begin_SpawnLeaders = FindFunction("State_begin_SpawnLeaders");
	UFunction* fstate_update_SpawnLeaders = FindFunction("State_update_SpawnLeaders");
	stateMachine->AddState(STATE_SPAWNLEADERS, fstate_begin_SpawnLeaders, fstate_update_SpawnLeaders, nullptr);

	UFunction* fstate_begin_CoinToss = FindFunction("State_begin_CoinToss");
	UFunction* fstate_update_CoinToss = FindFunction("State_update_CoinToss");
	stateMachine->AddState(STATE_COINTOSS, fstate_begin_CoinToss, fstate_update_CoinToss, nullptr);

	UFunction* fstate_begin_Mulligan = FindFunction("State_begin_Mulligan");
	UFunction* fstate_update_Mulligan = FindFunction("State_update_Mulligan");
	stateMachine->AddState(STATE_MULLIGAN, fstate_begin_Mulligan, fstate_update_Mulligan, nullptr);

	UFunction* fstate_begin_RoundStart = FindFunction("State_begin_RoundStart");
	UFunction* fstate_update_RoundStart = FindFunction("State_update_RoundStart");
	stateMachine->AddState(STATE_ROUNDSTART, fstate_begin_RoundStart, fstate_update_RoundStart, nullptr);

	UFunction* fstate_begin_PlayerTurn = FindFunction("State_begin_PlayerTurn");
	UFunction* fstate_update_PlayerTurn = FindFunction("State_update_PlayerTurn");
	UFunction* fstate_leave_PlayerTurn = FindFunction("State_leave_PlayerTurn");
	stateMachine->AddState(STATE_PLAYERTURN, fstate_begin_PlayerTurn, fstate_update_PlayerTurn, fstate_leave_PlayerTurn);

	UFunction* fstate_begin_ChangingPlayer = FindFunction("State_begin_ChangingPlayer");
	UFunction* fstate_update_ChangingPlayer = FindFunction("State_update_ChangingPlayer");
	stateMachine->AddState(STATE_CHANGINGPLAYER, fstate_begin_ChangingPlayer, fstate_update_ChangingPlayer, nullptr);

	UFunction* fstate_begin_ShowingRoundResult = FindFunction("State_begin_ShowingRoundResult");
	UFunction* fstate_update_ShowingRoundResult = FindFunction("State_update_ShowingRoundResult");
	stateMachine->AddState(STATE_SHOWINGROUNDRESULT, fstate_begin_ShowingRoundResult, fstate_update_ShowingRoundResult, nullptr);

	UFunction* fstate_begin_ClearingBoard = FindFunction("State_begin_ClearingBoard");
	UFunction* fstate_update_ClearingBoard = FindFunction("State_update_ClearingBoard");
	UFunction* fstate_leave_ClearingBoard = FindFunction("State_leave_ClearingBoard");
	stateMachine->AddState(STATE_CLEARINGBOARD, fstate_begin_ClearingBoard, fstate_update_ClearingBoard, fstate_leave_ClearingBoard);

	UFunction* fstate_begin_ShowingFinalResult = FindFunction("State_begin_ShowingFinalResult");
	UFunction* fstate_update_ShowingFinalResult = FindFunction("State_update_ShowingFinalResult");
	stateMachine->AddState(STATE_SHOWINGFINALRESULT, fstate_begin_ShowingFinalResult, fstate_update_ShowingFinalResult, nullptr);

	UFunction* fstate_begin_Reset = FindFunction("State_begin_Reset");
	stateMachine->AddState(STATE_RESET, fstate_begin_Reset, nullptr, nullptr);

	UHumanPlayerController* _playerController1 = NewObject<UHumanPlayerController>();
	_playerController1->AddToRoot();
	_playerController1->nPlayer = PLAYER_1;
	_playerController1->nOpponent = PLAYER_2;

	UAIPlayerController* _playerController2 = NewObject<UAIPlayerController>();
	_playerController2->AddToRoot();
	_playerController2->nPlayer = PLAYER_2;
	_playerController2->nOpponent = PLAYER_1;

	playerControllers.Add(_playerController1);
	playerControllers.Add(_playerController2);

	// 	int32 i = stateMachine->stateList.Num();
	// 	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::FromInt(i));

	nCurrentRound = 0;
}

void UCardsGameFlowController::State_update_Initializing()
{
	stateMachine->ChangeState(STATE_TUTORIALS);
}

void UCardsGameFlowController::State_leave_Initializing()
{
	if (!GetCardManager())
	{

		LogError("GFX --- Tried to link reference to card manager after initializing, was unable to!");


	}

	if (playerControllers[PLAYER_1]->IsA(UHumanPlayerController::StaticClass()))
	{
		//TODO ChoiceDialog
		//Cast<UHumanPlayerController>(playerControllers[PLAYER_1])->SetChoiceDialog(mcChoiceDialog);
	}
}

void UCardsGameFlowController::State_begin_Tutorials()
{

}

void UCardsGameFlowController::State_update_Tutorials()
{
	stateMachine->ChangeState(STATE_SPAWNLEADERS);
}

void UCardsGameFlowController::State_begin_SpawnLeaders()
{

	LogWarning("GFX ##########################################################");
	LogWarning("GFX -#AI#-----------------------------------------------------------------------------------------------------");
	LogWarning("GFX -#AI#----------------------------- NEW CARDS GAME ------------------------------------");
	LogWarning("GFX -#AI#-----------------------------------------------------------------------------------------------------");



	GetCardManager()->SpawnLeaders();

	bRoundStart = false;
	bPlayedThreeHeroesOneRound = false;

	UCardLeaderInstance* leaderP1 = GetCardManager()->GetCardLeader(PLAYER_1);
	UCardLeaderInstance* leaderP2 = GetCardManager()->GetCardLeader(PLAYER_2);

	//TODO visual

	playerControllers[PLAYER_1]->bCardZoomEnabled = false;
	playerControllers[PLAYER_2]->bCardZoomEnabled = false;

	if (leaderP1 && leaderP2 && leaderP1->nTemplateId != leaderP2->nTemplateId)
	{
		if (leaderP1->templateRef->GetFirstEffect() == CardEffect_Counter_King || leaderP2->templateRef->GetFirstEffect() == CardEffect_Counter_King)
		{
			if (leaderP1->templateRef->GetFirstEffect() != leaderP2->templateRef->GetFirstEffect())
			{
				if (leaderP1->templateRef->GetFirstEffect() == CardEffect_Counter_King)
				{
					//TODO visual

					LogWarning("P1: cards_player_counter_leader | gui_cards_using_ability");


				}
				else
				{
					//same as above

					LogWarning("P2: cards_player_counter_leader | gui_cards_using_ability");


				}
			}

			leaderP1->bCanBeUsed = false;
			GetBoardRenderer()->DisableLeaderUse(PLAYER_1);

			leaderP2->bCanBeUsed = false;
			GetBoardRenderer()->DisableLeaderUse(PLAYER_2);

			//TODO visual
		}
	}

	playerControllers[PLAYER_1]->SetCurrentRoundStatus(ROUND_PLAYER_STATUS_ACTIVE);
	playerControllers[PLAYER_2]->SetCurrentRoundStatus(ROUND_PLAYER_STATUS_ACTIVE);
}

void UCardsGameFlowController::State_update_SpawnLeaders()
{
	stateMachine->ChangeState(STATE_COINTOSS);
}

void UCardsGameFlowController::State_begin_CoinToss()
{
	int32 nDeckFactionP1 = GetCardManager()->playerDeckDefinitions[PLAYER_1]->GetDeckFaction();
	int32 nDeckFactionP2 = GetCardManager()->playerDeckDefinitions[PLAYER_2]->GetDeckFaction();


	LogWarning("GFX - Flip coin logic, player1 faction: " + FString::FromInt(nDeckFactionP1) + " , player2 faction:" + FString::FromInt(nDeckFactionP2));



	if (nDeckFactionP1 != nDeckFactionP2 && (nDeckFactionP1 == FactionId_Scoiatael || nDeckFactionP2 == FactionId_Scoiatael))
	{
		//TODO sound
		if (nDeckFactionP1 == FactionId_Scoiatael) //Human
		{
			nCurrentPlayer = PLAYER_INVALID;

			LogWarning("P1 faction is Scoiatael");


			//COIN_TOSS_POPUP_NEEDED event not needed
		}
		else //AI
		{
			nCurrentPlayer = PLAYER_2;

			LogWarning("P2 faction is Scoiatael: cards_opponent_scoiatael_start_special");


		}
	}
	else
	{
		//TODO if tutorial, P1 always starts first
		nCurrentPlayer = FPlatformMath::FloorToInt(UKismetMathLibrary::RandomFloatInRange(0, 1) * 2);
		if (nCurrentPlayer >= 2) nCurrentPlayer--;


		LogWarning("CoinToss result is " + FString::FromInt(nCurrentPlayer));




		if (nCurrentPlayer == PLAYER_1)
		{

			LogWarning("P1 starts! cards_player_will_go_first_message | coin_flip_win");


		}
		else if (nCurrentPlayer == PLAYER_2)
		{

			LogWarning("P2 starts! cards_player_will_go_first_message | coin_flip_loss");


		}

		//play sound
		GetBoardRenderer()->PlaySound("CoinToss");
	}

	QueueMessage(STATE_COINTOSS, nCurrentPlayer);
}

void UCardsGameFlowController::State_update_CoinToss()
{
	if (!IsShowingMessage())
	{
		if (nCurrentPlayer != PLAYER_INVALID)
		{
			stateMachine->ChangeState(STATE_MULLIGAN);
		}

		else
		{
			LogError("PLAYER_INVALID!!!");
		}

	}
}

void UCardsGameFlowController::State_begin_Mulligan()
{

	LogWarning("//TODO Implement Begin_Mulligan");


	GetCardManager()->ShuffleAndDrawCards();

	//TODO _mulliganDecided
	_bMulliganDecided = true;
}

void UCardsGameFlowController::State_update_Mulligan()
{

	LogWarning("//TODO Implement Update_Mulligan");



	if (_bMulliganDecided)
	{
		//TODO _mulliganDecided
		stateMachine->ChangeState(STATE_ROUNDSTART);
		bRoundStart = true;
		playerControllers[PLAYER_1]->bCardZoomEnabled = true;
		playerControllers[PLAYER_2]->bCardZoomEnabled = true;

		//TODO play sound game starts
	}
}

void UCardsGameFlowController::State_begin_RoundStart()
{

	LogWarning("cards_round_start | round_start");



	GetCardManager()->cardEffectManager->FlushAllEffects();

	bAllNeutralInRound = true;
	bPlayedCreaturesInRound = false;

	if (nLastRoundWinner != PLAYER_INVALID && GetCardManager()->playerDeckDefinitions[nLastRoundWinner]->GetDeckFaction() == FactionId_Northern_Kingdom)
	{
		//TODO visual

		LogWarning("cards_northern_ability_triggered | gui_cards_northern_realms_ability");



		GetCardManager()->DrawCard(nLastRoundWinner);

		GetBoardRenderer()->PlaySound("DealCard");
		QueueMessage(STATE_ABILITY_NORTHERN, PLAYER_INVALID);

	}

	GetBoardRenderer()->PlaySound("RoundStart");
	QueueMessage(STATE_ROUNDSTART, nCurrentPlayer);
}

void UCardsGameFlowController::State_update_RoundStart()
{
	//TODO Tutorials related

	if (!IsShowingMessage())
	{
		playerControllers[PLAYER_1]->ResetCurrentRoundStatus();
		playerControllers[PLAYER_2]->ResetCurrentRoundStatus();

		if (playerControllers[nCurrentPlayer]->GetCurrentRoundStatus() == ROUND_PLAYER_STATUS_DONE)
		{
			nCurrentPlayer = nCurrentPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1);

			if (playerControllers[nCurrentPlayer]->GetCurrentRoundStatus() == ROUND_PLAYER_STATUS_DONE)
			{
				stateMachine->ChangeState(STATE_SHOWINGROUNDRESULT);

				LogWarning("[[Both players passed, showing results...");


			}
			else
			{
				stateMachine->ChangeState(STATE_PLAYERTURN);

				LogWarning("[[Player passed, switching players...");


			}
		}
		else
		{
			stateMachine->ChangeState(STATE_PLAYERTURN);

			LogWarning("[[New Round started, winner starts...");


		}
	}
}

void UCardsGameFlowController::State_begin_PlayerTurn()
{

	LogWarning("GFX -#AI# starting player turn for player: " + FString::FromInt(nCurrentPlayer));



	if (nCurrentPlayer == PLAYER_1)
	{
		if (GetCardManager()->GetCardLeader(PLAYER_1)->bCanBeUsed)
		{
			GetBoardRenderer()->P1LeaderUse->SetVisibility(ESlateVisibility::HitTestInvisible);
			GetBoardRenderer()->P2LeaderUse->SetVisibility(ESlateVisibility::Hidden);
		}
		else GetBoardRenderer()->P1LeaderUse->SetVisibility(ESlateVisibility::Hidden);

		LogWarning("[[cards_player_turn_start_message]] | your_turn");
	}
	else if (nCurrentPlayer == PLAYER_2)
	{
		if (GetCardManager()->GetCardLeader(PLAYER_2)->bCanBeUsed)
		{
			GetBoardRenderer()->P2LeaderUse->SetVisibility(ESlateVisibility::HitTestInvisible);
			GetBoardRenderer()->P1LeaderUse->SetVisibility(ESlateVisibility::Hidden);
		}
		else GetBoardRenderer()->P2LeaderUse->SetVisibility(ESlateVisibility::Hidden);

		LogWarning("[[cards_opponent_turn_start_message]] | Opponents_turn");
	}

	//TODO tutorial and visual

	//added here, pass if needed DHK
	if (GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nCurrentPlayer).Num() == 0 &&
		GetCardManager()->GetCardLeader(nCurrentPlayer) != nullptr &&
		!GetCardManager()->GetCardLeader(nCurrentPlayer)->bCanBeUsed)
	{
		if (nCurrentPlayer == PLAYER_1)
			Cast<UHumanPlayerController>(playerControllers[nCurrentPlayer])->SkipTurn();
		else
			Cast<UAIPlayerController>(playerControllers[nCurrentPlayer])->SkipTurn();

		return;
	}

	playerControllers[nCurrentPlayer]->bInputEnabled = false; //DHK
	playerControllers[nCurrentPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1)]->bInputEnabled = false;

	GetBoardRenderer()->PlaySound("PlayerTurn");
	QueueMessage(STATE_PLAYERTURN, nCurrentPlayer);
}

void UCardsGameFlowController::State_update_PlayerTurn()
{
	if (!IsShowingMessage())
	{
		UBasePlayerController* currentPlayerController = playerControllers[nCurrentPlayer];
		UBasePlayerController* opponentController = playerControllers[nCurrentPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1)];

		if (!currentPlayerController)
		{
			LogError("GFX ---- currentPlayerController not found for player: " + FString::FromInt(nCurrentPlayer));
		}

		if (currentPlayerController->bTurnOver)
		{
			//TODO tutorials
			if (currentPlayerController->GetCurrentRoundStatus() != ROUND_PLAYER_STATUS_ACTIVE)
			{
				if (opponentController->GetCurrentRoundStatus() != ROUND_PLAYER_STATUS_ACTIVE)
				{
					stateMachine->ChangeState(STATE_SHOWINGROUNDRESULT);
				}
				else
				{
					stateMachine->ChangeState(STATE_CHANGINGPLAYER);
				}
			}
			else if (opponentController->GetCurrentRoundStatus() != ROUND_PLAYER_STATUS_DONE && currentPlayerController->bHandPlayed)
			{
				stateMachine->ChangeState(STATE_CHANGINGPLAYER);
			}
			else
			{
				currentPlayerController->StartTurn();
			}
		}
	}
}

void UCardsGameFlowController::State_leave_PlayerTurn()
{

	LogWarning("State_leave_PlayerTurn!");


	TArray<UCardInstance*> allCreaturesList;
	UCardTemplate* cardTemplate = nullptr;
	int32 nCounter = 0;

	//playerControllers[nCurrentPlayer].playerRenderer.turnActive = false;

	if (bAllNeutralInRound || !bPlayedCreaturesInRound)
	{
		allCreaturesList.Empty();
		allCreaturesList = GetCardManager()->GetAllCreatures(PLAYER_1);
		nCounter = 0;

		while (nCounter < allCreaturesList.Num())
		{
			cardTemplate = allCreaturesList[nCounter]->templateRef;
			if (!cardTemplate->IsType(CardType_Spy))
			{
				bPlayedCreaturesInRound = true;
				if (cardTemplate->nFactionIdx != FactionId_Neutral)
				{
					bAllNeutralInRound = false;
				}
			}
			nCounter++;
		}

		allCreaturesList.Empty();
		allCreaturesList = GetCardManager()->GetAllCreatures(PLAYER_2);
		nCounter = 0;

		while (nCounter < allCreaturesList.Num())
		{
			cardTemplate = allCreaturesList[nCounter]->templateRef;
			if (cardTemplate->IsType(CardType_Spy))
			{
				bPlayedCreaturesInRound = true;
				if (cardTemplate->nFactionIdx != FactionId_Neutral)
				{
					bAllNeutralInRound = false;
				}
			}
			nCounter++;
		}
	}
}

void UCardsGameFlowController::State_begin_ChangingPlayer()
{
	if (playerControllers[nCurrentPlayer]->GetCurrentRoundStatus() == ROUND_PLAYER_STATUS_DONE)
	{
		if (nCurrentPlayer == PLAYER_1)
		{
			//TODO push message and show message bool
			LogWarning("[[cards_player_passed_turn]] | passed");
		}
		else
		{
			LogWarning("[[cards_opponent_passed_turn]] | passed");
		}

		QueueMessage(STATE_ROUNDPASSED, nCurrentPlayer);
	}
}

void UCardsGameFlowController::State_update_ChangingPlayer()
{
	if (!IsShowingMessage())
	{
		//TODO visual
		nCurrentPlayer = nCurrentPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1);

		UBasePlayerController* currentPlayerController = playerControllers[nCurrentPlayer];
		UBasePlayerController* opponentController = playerControllers[nCurrentPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1)];

		currentPlayerController->bHandPlayed = false;
		opponentController->bHandPlayed = false;

		stateMachine->ChangeState(STATE_PLAYERTURN);
	}
}

void UCardsGameFlowController::State_begin_ShowingRoundResult()
{
	int32 nCurrentScoreP1 = GetCardManager()->currentPlayerScores[PLAYER_1];
	int32 nCurrentScoreP2 = GetCardManager()->currentPlayerScores[PLAYER_2];

	int32 nDeckFactionP1 = GetCardManager()->playerDeckDefinitions[PLAYER_1]->GetDeckFaction();
	int32 nDeckFactionP2 = GetCardManager()->playerDeckDefinitions[PLAYER_2]->GetDeckFaction();

	int32 nPlayer = PLAYER_INVALID;

	playerControllers[PLAYER_1]->ResetCurrentRoundStatus();
	playerControllers[PLAYER_2]->ResetCurrentRoundStatus();

	//TODO tutorials

	if (nCurrentScoreP1 == nCurrentScoreP2)
	{
		if (nDeckFactionP1 != nDeckFactionP2 && (nDeckFactionP1 == FactionId_Nilfgaard || nDeckFactionP2 == FactionId_Nilfgaard))
		{
			//TODO visual and sound

			LogWarning("[[cards_nilfgaard_ability_triggered]] | nilf_ability");



			QueueMessage(STATE_ABILITY_NILFGAARD, PLAYER_INVALID);

			if (nDeckFactionP1 == FactionId_Nilfgaard)
			{

				LogWarning("[[cards_player_won_round]] | battle_won");


				nPlayer = PLAYER_1;
				nLastRoundWinner = PLAYER_1;

				GetBoardRenderer()->PlaySound("RoundWon");
				QueueMessage(STATE_ROUNDWON, PLAYER_INVALID);
			}
			else
			{

				LogWarning("[[cards_opponent_won_round]] | battle_lost");


				nPlayer = PLAYER_2;
				nLastRoundWinner = PLAYER_2;

				GetBoardRenderer()->PlaySound("RoundLost");
				QueueMessage(STATE_ROUNDLOST, PLAYER_INVALID);
			}
		}
		else
		{

			LogWarning("[[cards_round_draw]] | battle_draw");


			nPlayer = PLAYER_INVALID;
			nLastRoundWinner = PLAYER_INVALID;

			//TODO Battle draw visual and sound
		}
	}
	else if (nCurrentScoreP1 > nCurrentScoreP2)
	{

		LogWarning("[[cards_player_won_round]] | battle_won");


		nPlayer = PLAYER_1;
		nLastRoundWinner = PLAYER_1;

		GetBoardRenderer()->PlaySound("RoundWon");
		QueueMessage(STATE_ROUNDWON, PLAYER_INVALID);
	}
	else
	{

		LogWarning("[[cards_opponent_won_round]] | battle_lost");


		nPlayer = PLAYER_2;
		nLastRoundWinner = PLAYER_2;

		GetBoardRenderer()->PlaySound("RoundLost");
		QueueMessage(STATE_ROUNDLOST, PLAYER_INVALID);
	}
	if (nPlayer != PLAYER_INVALID)
	{
		//CardsGameMain.mSingleton.playSound("gui_cards_gem_destruction");
	}

	GetCardManager()->roundResults[nCurrentRound]->SetResults(nCurrentScoreP1, nCurrentScoreP2, nPlayer);

	GetCardManager()->TraceRoundResults();



	GetCardManager()->UpdatePlayerLives();

	TArray<UCardInstance*> allCreaturesListP1 = GetCardManager()->GetAllCreatures(PLAYER_1);

	int32 nHero = 0;

	int32 nIndex = 0;
	while (nIndex < allCreaturesListP1.Num())
	{
		if (allCreaturesListP1[nIndex]->templateRef->IsType(CardType_Hero))
		{
			nHero++;
		}
		nIndex++;
	}

	if (nHero >= 3)
	{
		bPlayedThreeHeroesOneRound = true;
	}

	if (bAllNeutralInRound && bPlayedCreaturesInRound && nPlayer == PLAYER_1)
	{
		//TODO sendNeutralRoundVictoryAchievement
		//CardsGameMain.mSingleton.sendNeutralRoundVictoryAchievement();
	}
}

void UCardsGameFlowController::State_update_ShowingRoundResult()
{
	//if (!mcMessageQueue.ShowingMessage())
	{
		if (nCurrentRound == 2
			|| nCurrentRound == 1 && (GetCardManager()->roundResults[0]->GetRoundWinningPlayer() ==
				GetCardManager()->roundResults[1]->GetRoundWinningPlayer()
				|| GetCardManager()->roundResults[0]->GetRoundWinningPlayer() == PLAYER_INVALID
				|| GetCardManager()->roundResults[1]->GetRoundWinningPlayer() == PLAYER_INVALID))
		{
			GetCardManager()->ClearBoard(false);
			stateMachine->ChangeState(STATE_SHOWINGFINALRESULT);
		}
		else
		{
			if (nLastRoundWinner != PLAYER_INVALID)
			{
				nCurrentPlayer = nLastRoundWinner;
			}
			stateMachine->ChangeState(STATE_CLEARINGBOARD);
		}
	}
}

void UCardsGameFlowController::State_begin_ClearingBoard()
{
	bool bIsDeckMonster = false;

	if (GetCardManager()->playerDeckDefinitions[PLAYER_1]->GetDeckFaction() == FactionId_No_Mans_Land && GetCardManager()->ChooseCreatureToExclude(PLAYER_1) != nullptr)
	{
		bIsDeckMonster = true;
	}
	else if (!bIsDeckMonster && GetCardManager()->playerDeckDefinitions[PLAYER_2]->GetDeckFaction() == FactionId_No_Mans_Land && GetCardManager()->ChooseCreatureToExclude(PLAYER_2) != nullptr)
	{
		bIsDeckMonster = true;
	}

	if (bIsDeckMonster)
	{
		//TODO visual and sound
		LogWarning("[[cards_monster_faction_ability_triggered]] | monster_ability");
	}

	GetCardManager()->ClearBoard(true);
}

void UCardsGameFlowController::State_update_ClearingBoard()
{
	if (!IsShowingMessage())
	{
		GetCardManager()->RecalculateScores();

		nCurrentRound++;

		stateMachine->ChangeState(STATE_ROUNDSTART);
	}
}

void UCardsGameFlowController::State_leave_ClearingBoard()
{
	GetCardManager()->RecalculateScores(); //is in update as well?!?
}

void UCardsGameFlowController::State_begin_ShowingFinalResult()
{
	int32 nDeckFactionP1 = 0;
	int32 nDeckFactionP2 = 0;

	int32 nWinner = PLAYER_INVALID;

	int32 nWinnerRound0 = GetCardManager()->roundResults[0]->GetRoundWinningPlayer();
	int32 nWinnerRound1 = GetCardManager()->roundResults[1]->GetRoundWinningPlayer();

	if (nCurrentRound == 1 && nWinnerRound0 != nWinnerRound1 &&
		!GetCardManager()->roundResults[2]->Played())
	{
		nDeckFactionP1 = GetCardManager()->playerDeckDefinitions[PLAYER_1]->GetDeckFaction();
		nDeckFactionP2 = GetCardManager()->playerDeckDefinitions[PLAYER_2]->GetDeckFaction();

		if (nDeckFactionP1 != nDeckFactionP2)
		{
			if (nDeckFactionP1 == FactionId_Nilfgaard)
			{
				GetCardManager()->roundResults[2]->SetResults(0, 0, PLAYER_1);
			}
			else if (nDeckFactionP2 == FactionId_Nilfgaard)
			{
				GetCardManager()->roundResults[2]->SetResults(0, 0, PLAYER_2);
			}
		}
	}

	//TODO tutorial

	int32 nWinnerRound2 = GetCardManager()->roundResults[2]->GetRoundWinningPlayer();

	playerControllers[PLAYER_1]->bCardZoomEnabled = false;
	playerControllers[PLAYER_2]->bCardZoomEnabled = false;

	//mcChoiceDialog.hideDialog();

	if (nCurrentRound == 1 &&
		(nWinnerRound0 == nWinnerRound1 || nWinnerRound0 == PLAYER_INVALID
			|| nWinnerRound1 == PLAYER_INVALID))
	{
		if (nWinnerRound0 == PLAYER_INVALID)
		{
			nWinner = nWinnerRound1;
		}
		else
		{
			nWinner = nWinnerRound0;
		}
	}
	else if (nCurrentRound == 2)
	{
		if (nWinnerRound0 == nWinnerRound1 || nWinnerRound0 == nWinnerRound2)
		{
			nWinner = nWinnerRound0;
		}
		else if (nWinnerRound1 == nWinnerRound2)
		{
			nWinner = nWinnerRound1;
		}
	}
	else
	{
		LogError("GFX - Danger Will Robinson, danger!");
	}


	GetCardManager()->TraceRoundResults();
	LogWarning("GFX -#AI#--- game winner was: " + FString::FromInt(nWinner));
	LogWarning("GFX -#AI#--- current round was: " + FString::FromInt(nCurrentRound));
	LogWarning("GFX -#AI#--- Round 1 winner: " + FString::FromInt(nWinnerRound0));
	LogWarning("GFX -#AI#--- Round 2 winner: " + FString::FromInt(nWinnerRound0));
	LogWarning("GFX -#AI#--- Round 3 winner: " + FString::FromInt(nWinnerRound0));



	if (nWinner == PLAYER_1)
	{
		if (bPlayedThreeHeroesOneRound)
		{
			//TODO sendHeroRoundVictoryAchievement
		}
		//TODO sound gui_cards_battle_won

		LogWarning("gui_cards_battle_won");



	}
	else if (nWinner == PLAYER_2)
	{

		LogWarning("gui_cards_battle_lost");


	}
	else
	{

		LogWarning("gui_cards_battle_draw");


	}

	//TODO show end game result, include round vs. game 0/1

	OnEndGameResult(nWinner);//round
}

void UCardsGameFlowController::State_update_ShowingFinalResult()
{
}

void UCardsGameFlowController::State_begin_Reset()
{
	//TODO reset
	nCurrentRound = 0;
	GetCardManager()->Reset();
	LogWarning("TODO [[cards_resetting]]");
	stateMachine->ChangeState(STATE_SPAWNLEADERS);
}

void UCardsGameFlowController::QueueMessage(int32 nState, int32 nPlayer)
{
	if (messageQueue.Num() == 0) //start
	{
		messageQueue.Add(nState);
		GetBoardRenderer()->HandleChangeStates(messageQueue[0], nCurrentPlayer, false);

		GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().SetTimer(TimerHandleDelayAction, this, &UCardsGameFlowController::OnShowMessageEnd, 1.2, false);
	}
	else messageQueue.Add(nState); //queue
}

void UCardsGameFlowController::OnEndGameResult(int32 nWinner)
{
	if (nWinner == PLAYER_INVALID)
	{

		LogWarning("HUH?!?[[OnEndGameResult PLAYER_INVALID]]");

	}
	else if (nWinner == PLAYER_BOTH)
	{

		LogWarning("HUH?!?[[OnEndGameResult PLAYER_BOTH]]");//TODO Maybe DRAW here?

	}
	else //we have a winner
	{

		LogWarning("TODO draw games [[EndGameDialogResult_End]]");


		FString sResult = nWinner == PLAYER_1 ? "VICTORY" : "DEFEAT";

		GetCurrentPlayerController()->UIGameResults->SetVisibility(ESlateVisibility::Visible);
		GetCurrentPlayerController()->UIGameResults->GameResultText->SetText(FText::FromString(sResult));

		if (sResult == "DEFEAT")
		{
			GetCurrentPlayerController()->UIGameResults->GameResultText->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
			GetCurrentPlayerController()->UIGameResults->PlayerText->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
			GetCurrentPlayerController()->UIGameResults->OpponentText->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
			GetCurrentGameInstance()->nLoses++;
		}
		else //VICTORY
		{
			GetCurrentPlayerController()->UIGameResults->GameResultText->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
			GetCurrentPlayerController()->UIGameResults->PlayerText->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
			GetCurrentPlayerController()->UIGameResults->OpponentText->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
			GetCurrentGameInstance()->nWins++;
		}

		for (int32 i = 0; i < GetCardManager()->roundResults.Num(); i++)
		{
			UCardsRoundResult* _result = GetCardManager()->roundResults[i];

			if (_result->GetRoundWinningPlayer() == PLAYER_1)
			{
				if (i == 0)
				{
					if (_result->GetRoundScores().Num() > 0)
					{
						GetCurrentPlayerController()->UIGameResults->PR1->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_1])));
						GetCurrentPlayerController()->UIGameResults->PR1->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
						GetCurrentPlayerController()->UIGameResults->OR1->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_2])));
						GetCurrentPlayerController()->UIGameResults->OR1->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
					}
				}
				else if (i == 1)
				{
					if (_result->GetRoundScores().Num() > 0)
					{
						GetCurrentPlayerController()->UIGameResults->PR2->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_1])));
						GetCurrentPlayerController()->UIGameResults->PR2->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
						GetCurrentPlayerController()->UIGameResults->OR2->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_2])));
						GetCurrentPlayerController()->UIGameResults->OR2->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
					}
				}
				else
				{
					if (_result->GetRoundScores().Num() > 0)
					{
						GetCurrentPlayerController()->UIGameResults->PR3->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_1])));
						GetCurrentPlayerController()->UIGameResults->PR3->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
						GetCurrentPlayerController()->UIGameResults->OR3->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_2])));
						GetCurrentPlayerController()->UIGameResults->OR3->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
					}
				}
			}
			else
			{
				if (i == 0)
				{
					if (_result->GetRoundScores().Num() > 0)
					{
						GetCurrentPlayerController()->UIGameResults->PR1->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_1])));
						GetCurrentPlayerController()->UIGameResults->PR1->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
						GetCurrentPlayerController()->UIGameResults->OR1->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_2])));
						GetCurrentPlayerController()->UIGameResults->OR1->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
					}
				}
				else if (i == 1)
				{
					if (_result->GetRoundScores().Num() > 0)
					{
						GetCurrentPlayerController()->UIGameResults->PR2->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_1])));
						GetCurrentPlayerController()->UIGameResults->PR2->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
						GetCurrentPlayerController()->UIGameResults->OR2->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_2])));
						GetCurrentPlayerController()->UIGameResults->OR2->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
					}
				}
				else
				{
					if (_result->GetRoundScores().Num() > 0)
					{
						GetCurrentPlayerController()->UIGameResults->PR3->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_1])));
						GetCurrentPlayerController()->UIGameResults->PR3->SetColorAndOpacity(FLinearColor(1.f, 0.f, 0.f, 1.f));//RED
						GetCurrentPlayerController()->UIGameResults->OR3->SetText(FText::FromString(FString::FromInt(_result->GetRoundScores()[PLAYER_2])));
						GetCurrentPlayerController()->UIGameResults->OR3->SetColorAndOpacity(FLinearColor(0.955974f, 0.752942f, 0.412543f, 1.f));//GOLD
					}
				}
			}
		}

		stateMachine->ChangeState(STATE_IDLE);
		//stateMachine->ChangeState(STATE_RESET);

		//DHK TODO randomize rewards
		//save here DHK
		GetCurrentPlayerController()->UIDeckBuilder->SaveGame();
	}
}

bool UCardsGameFlowController::IsShowingMessage()
{
	return /*GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().IsTimerActive(TimerHandleDelayAction)
		|| */messageQueue.Num() > 0;
}

void  UCardsGameFlowController::OnShowMessageEnd()
{
	messageQueue.RemoveAt(0);
	if (messageQueue.Num() == 0)
	{
		GetBoardRenderer()->HandleChangeStates(STATE_INVALID, PLAYER_INVALID, true);
	}
	else
	{
		GetBoardRenderer()->HandleChangeStates(messageQueue[0], nCurrentPlayer, false);

		GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().SetTimer(TimerHandleDelayAction, this, &UCardsGameFlowController::OnShowMessageEnd, 1.2, false);
	}
}