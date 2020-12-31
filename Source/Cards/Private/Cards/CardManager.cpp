// Fill out your copyright notice in the Description page of Project Settings.

#include "CardManager.h"

#include "CardInstance.h"
#include "CardLeaderInstance.h"
#include "CardEffectManager.h"
#include "CardTemplate.h"

#include "CardsDeck.h"
#include "CardsGameMain.h"
#include "CardsPlayerController.h"

#include "ldf.h"

#include "Kismet/KismetMathLibrary.h"

UCardManager::UCardManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	cardEffectManager = NewObject<UCardEffectManager>();

	InitializeLists();
}

void UCardManager::SpawnLeaders()
{
	UCardLeaderInstance* leaderP1 = Cast<UCardLeaderInstance>(SpawnCardInstance(playerDeckDefinitions[PLAYER_1]->nSelectedKingIndex, PLAYER_1));
	AddCardInstanceToList(leaderP1, CARD_LIST_LOC_LEADER, PLAYER_1);


	LogWarning("Leader P1 ID is " + FString::FromInt(leaderP1->nTemplateId));



	UCardLeaderInstance* leaderP2 = Cast<UCardLeaderInstance>(SpawnCardInstance(playerDeckDefinitions[PLAYER_2]->nSelectedKingIndex, PLAYER_2));
	AddCardInstanceToList(leaderP2, CARD_LIST_LOC_LEADER, PLAYER_2);


	LogWarning("Leader P2 ID is " + FString::FromInt(leaderP2->nTemplateId));

}

void UCardManager::AddCardInstanceToList(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer)
{
	RemoveCardInstanceFromItsList(cardInstance);
	cardInstance->nInList = nCardListLocation;
	cardInstance->nListsPlayer = nPlayer;

	TArray<UCardInstance*>& cardInstanceList = GetCardInstanceList(nCardListLocation, nPlayer);


	LogWarning("GFX ====== Adding card with instance ID: "
		+ FString::FromInt(cardInstance->nInstanceId)
		+ " and power ["
		+ FString::FromInt(cardInstance->templateRef->nPower)
		+ "], to List " + ListIDToString(nCardListLocation)
		+ ", for player: " + FString::FromInt(nPlayer));



	cardInstanceList.Add(cardInstance);

	//visual
	GetBoardRenderer()->SpawnCardInstance(cardInstance, nCardListLocation, nPlayer);

	RecalculateScores();

	if (nCardListLocation == CARD_LIST_LOC_HAND)
	{
		GetBoardRenderer()->UpdateCardsInHand(nPlayer, cardInstanceList.Num());
	}
}

void UCardManager::AddCardInstanceIDToList(int32 nCardInstanceID, int32 nCardListLocation, int32 nPlayer)
{
	UCardInstance* cardInstance = GetCardInstance(nCardInstanceID);

	if (cardInstance)
	{
		AddCardInstanceToList(cardInstance, nCardListLocation, nPlayer);
	}
}

void UCardManager::RemoveCardInstanceFromList(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer)
{
	int32 nIndex = 0;

	if (cardInstance->nInList != CARD_LIST_LOC_INVALID)
	{
		cardInstance->nInList = CARD_LIST_LOC_INVALID;
		cardInstance->nListsPlayer = PLAYER_INVALID;

		TArray<UCardInstance*>& cardInstanceList = GetCardInstanceList(nCardListLocation, nPlayer);

		if (cardInstanceList.Num() == 0)
		{

			LogError("GFX - Tried to remove from unknown listID:" + FString::FromInt(nCardListLocation) + ", and player:" + FString::FromInt(nPlayer) + ", the following card: " + cardInstance->GetName());


		}

		nIndex = cardInstanceList.IndexOfByKey(cardInstance);
		if (nIndex < 0 || nIndex >= cardInstanceList.Num())
		{

			LogError("GFX - tried to remove card instance from a list that does not contain it: " + FString::FromInt(nCardListLocation) + ", " + FString::FromInt(nPlayer) + ", " + cardInstance->GetName());


		}

		cardInstanceList.RemoveAt(nIndex);

		GetBoardRenderer()->RemoveCardFromHolder(cardInstance);

		RecalculateScores();

		if (nCardListLocation == CARD_LIST_LOC_HAND)
		{
			GetBoardRenderer()->UpdateCardsInHand(nPlayer, cardInstanceList.Num());
		}
	}
}

void UCardManager::RemoveCardInstanceFromItsList(UCardInstance* cardInstance)
{
	RemoveCardInstanceFromList(cardInstance, cardInstance->nInList, cardInstance->nListsPlayer);
}

void UCardManager::RecalculateScores()
{
	int32 nWinner = GetWinningPlayer();

	int32 nP2Siege = CalculatePlayerScore(CARD_LIST_LOC_SIEGE, PLAYER_2);
	int32 nP2Ranged = CalculatePlayerScore(CARD_LIST_LOC_RANGED, PLAYER_2);
	int32 nP2Melee = CalculatePlayerScore(CARD_LIST_LOC_MELEE, PLAYER_2);

	int32 nP1Melee = CalculatePlayerScore(CARD_LIST_LOC_MELEE, PLAYER_1);
	int32 nP1Ranged = CalculatePlayerScore(CARD_LIST_LOC_RANGED, PLAYER_1);
	int32 nP1Siege = CalculatePlayerScore(CARD_LIST_LOC_SIEGE, PLAYER_1);

	currentPlayerScores[PLAYER_1] = nP1Melee + nP1Ranged + nP1Siege;
	currentPlayerScores[PLAYER_2] = nP2Melee + nP2Ranged + nP2Siege;

	GetBoardRenderer()->UpdateScore(currentPlayerScores[PLAYER_1], currentPlayerScores[PLAYER_2]);
	GetBoardRenderer()->UpdateRowScore(nP1Siege, nP1Ranged, nP1Melee, nP2Melee, nP2Ranged, nP2Siege);
}

void UCardManager::UpdatePlayerLives()
{
	int32 nIndex = 0;

	TArray<int32> playerLivesList;
	playerLivesList.Add(2);
	playerLivesList.Add(2);

	while (nIndex < roundResults.Num())
	{
		if (roundResults[nIndex]->Played())
		{
			if (roundResults[nIndex]->GetRoundWinningPlayer() == PLAYER_1 || roundResults[nIndex]->GetRoundWinningPlayer() == PLAYER_INVALID)
			{
				playerLivesList[PLAYER_2] = FMath::Max(0, (playerLivesList[PLAYER_2] - 1));
			}

			if (roundResults[nIndex]->GetRoundWinningPlayer() == PLAYER_2 || roundResults[nIndex]->GetRoundWinningPlayer() == PLAYER_INVALID)
			{
				playerLivesList[PLAYER_1] = FMath::Max(0, (playerLivesList[PLAYER_1] - 1));
			}
		}
		else
		{
			break;
		}

		++nIndex;

	}

	GetBoardRenderer()->SetPlayerLives(PLAYER_1, playerLivesList[PLAYER_1]);
	GetBoardRenderer()->SetPlayerLives(PLAYER_2, playerLivesList[PLAYER_2]);
}

void UCardManager::ClearBoard(bool bExclude)
{
	int32 nPlayer = 0;
	UCardInstance* cardExclude = nullptr;
	UCardInstance* cardWeather = nullptr;

	while (cardListWeather.Num() > 0)
	{
		cardWeather = cardListWeather[0];
		AddCardInstanceToList(cardWeather, CARD_LIST_LOC_GRAVEYARD, cardWeather->nOwningPlayer);
	}
	nPlayer = PLAYER_1;
	while (nPlayer <= PLAYER_2)
	{
		if (bExclude)
		{
			cardExclude = ChooseCreatureToExclude(nPlayer);
		}

		SendListToGraveyard(CARD_LIST_LOC_MELEE, nPlayer, cardExclude);
		SendListToGraveyard(CARD_LIST_LOC_RANGED, nPlayer, cardExclude);
		SendListToGraveyard(CARD_LIST_LOC_SIEGE, nPlayer, cardExclude);

		SendListToGraveyard(CARD_LIST_LOC_MELEEMODIFIERS, nPlayer, cardExclude);
		SendListToGraveyard(CARD_LIST_LOC_RANGEDMODIFIERS, nPlayer, cardExclude);
		SendListToGraveyard(CARD_LIST_LOC_SIEGEMODIFIERS, nPlayer, cardExclude);

		++nPlayer;
	}
}

void UCardManager::SendListToGraveyard(int32 nCardListLocation, int32 nPlayer, UCardInstance* cardInstance)
{
	TArray<UCardInstance*>& cardList = GetCardInstanceList(nCardListLocation, nPlayer);
	UCardInstance* cardInstanceRef = nullptr;
	int32 nIndex = 0;

	while (nIndex < cardList.Num())
	{
		cardInstanceRef = cardList[nIndex];

		if (cardInstanceRef == cardInstance)
		{
			nIndex++;
			continue;
		}

		if (nPlayer == PLAYER_INVALID)
		{
			AddCardInstanceToList(cardInstanceRef, CARD_LIST_LOC_GRAVEYARD, cardInstanceRef->nOwningPlayer);
			continue;
		}

		AddCardInstanceToList(cardInstanceRef, CARD_LIST_LOC_GRAVEYARD, cardInstanceRef->nListsPlayer);
	}
}

void UCardManager::SendToGraveyard(UCardInstance* cardInstance)
{
	if (cardInstance)
	{
		if (cardInstance->templateRef->IsType(CardType_Weather))
		{
			AddCardInstanceToList(cardInstance, CARD_LIST_LOC_GRAVEYARD, cardInstance->nOwningPlayer);
		}
		else
		{
			AddCardInstanceToList(cardInstance, CARD_LIST_LOC_GRAVEYARD, cardInstance->nListsPlayer);
		}
	}
}

void UCardManager::CalculateCardPowerPotentials()
{
	//P1
	TArray<UCardInstance*> cardsHandListP1 = GetCardInstanceList(CARD_LIST_LOC_HAND, PLAYER_1);

	int32 nCounter = 0;
	while (nCounter < cardsHandListP1.Num())
	{
		cardsHandListP1[nCounter]->RecalculatePowerPotential(this);
		nCounter++;
	}

	//P2
	TArray<UCardInstance*> cardsHandListP2 = GetCardInstanceList(CARD_LIST_LOC_HAND, PLAYER_2);

	nCounter = 0;
	while (nCounter < cardsHandListP2.Num())
	{
		cardsHandListP2[nCounter]->RecalculatePowerPotential(this);
		nCounter++;
	}
}

void UCardManager::ShuffleAndDrawCards()
{
	int32 nCardCounter = 0;

	UCardsDeck* cardsDeckP1 = playerDeckDefinitions[PLAYER_1];
	UCardsDeck* cardsDeckP2 = playerDeckDefinitions[PLAYER_2];

	UCardLeaderInstance* cardLeaderP1 = GetCardLeader(PLAYER_1);
	UCardLeaderInstance* cardLeaderP2 = GetCardLeader(PLAYER_2);


	if (cardsDeckP1->GetDeckKingTemplate() == nullptr && cardsDeckP2->GetDeckKingTemplate() == nullptr)
	{
		LogError("GFX - Trying to shuffle and draw cards when the decks are null!");
	}
	else if (cardsDeckP1->GetDeckKingTemplate() == nullptr)
	{
		LogError("GFX - Trying to shuffle and draw cards when one of the following decks is null: P1");
	}
	else if (cardsDeckP2->GetDeckKingTemplate() == nullptr)
	{
		LogError("GFX - Trying to shuffle and draw cards when one of the following decks is null: P2");
	}



	LogWarning("GFX -#AI#------------------- DECK STRENGTH --------------------");
	LogWarning("GFX -#AI#--- PLAYER 1:");



	cardsDeckP1->ShuffleDeck(cardsDeckP2->OriginalStength());


	LogWarning("GFX -#AI#--- PLAYER 2:");



	cardsDeckP2->ShuffleDeck(cardsDeckP1->OriginalStength());


	LogWarning("GFX -#AI#------------------------------------------------------");



	//P1
	if (cardLeaderP1->bCanBeUsed && cardLeaderP1->templateRef->GetFirstEffect() == CardEffect_11th_card)
	{
		cardLeaderP1->bCanBeUsed = false;
		GetBoardRenderer()->DisableLeaderUse(PLAYER_1);

		nCardCounter = 11;
	}
	else
	{
		nCardCounter = 10;
	}

	LogWarning("TODO _TEMP_tutorialsOn");


	if (_TEMP_tutorialsOn) //TODO
	{
		if (TryDrawSpecificCard(PLAYER_1, 3))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 5))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 150))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 115))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 135))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 111))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 145))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 113))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 114))
		{
			nCardCounter--;
		}
		if (TryDrawSpecificCard(PLAYER_1, 107))
		{
			nCardCounter--;
		}
		//TODO sound
		LogWarning("P1 gui_cards_draw_card");
	}

	DrawCards(PLAYER_1, nCardCounter);

	TArray<UCardInstance*>& cardsHandListP1 = GetCardInstanceList(CARD_LIST_LOC_HAND, PLAYER_1);
	cardsHandListP1.Sort(CardSorter);

	//P2
	if (cardLeaderP2->bCanBeUsed && cardLeaderP2->templateRef->GetFirstEffect() == CardEffect_11th_card)
	{
		cardLeaderP2->bCanBeUsed = false;
		GetBoardRenderer()->DisableLeaderUse(PLAYER_2);

		nCardCounter = 11;
	}
	else
	{
		nCardCounter = 10;
	}

	DrawCards(PLAYER_2, nCardCounter);


	LogWarning("ShuffleAndDrawCards ready!");



}

void UCardManager::SummonFromHand(int32 nPlayer, int32 nCardIndex)
{
	int32 nCounter = 0;

	TArray<UCardInstance*> cardsHandList;
	cardsHandList = GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer);

	UCardInstance* cardInstance = nullptr;

	while (nCounter < cardsHandList.Num())
	{
		cardInstance = cardsHandList[nCounter];

		if (cardInstance->nTemplateId == nCardIndex)
		{
			//TODO visual summon fx
			/*cardInstance->playSummonedFX = true;*/

			if (cardInstance->templateRef->IsType(CardType_Melee))
			{
				AddCardInstanceToList(cardInstance, CARD_LIST_LOC_MELEE, nPlayer);
			}
			else if (cardInstance->templateRef->IsType(CardType_Ranged))
			{
				AddCardInstanceToList(cardInstance, CARD_LIST_LOC_RANGED, nPlayer);
			}
			else if (cardInstance->templateRef->IsType(CardType_Siege))
			{
				AddCardInstanceToList(cardInstance, CARD_LIST_LOC_SIEGE, nPlayer);
			}

			continue;
		}

		++nCounter;
	}
}

UCardInstance* UCardManager::SpawnCardInstance(int32 nCardIndex, int32 nPlayer, int32 nCardListLocation)
{
	UCardInstance* cardInstance = nullptr;

	nLastInstance++;

	if (nCardIndex >= 1000) //leader
		cardInstance = NewObject<UCardLeaderInstance>();
	else cardInstance = NewObject<UCardInstance>();

	cardInstance->AddToRoot();

	if (nCardListLocation == CARD_LIST_LOC_INVALID)
	{
		nCardListLocation = CARD_LIST_LOC_DECK;
	}

	cardInstance->nTemplateId = nCardIndex;
	cardInstance->templateRef = GetCardTemplate(nCardIndex);
	cardInstance->nOwningPlayer = nPlayer;
	cardInstance->nInstanceId = nLastInstance;

	_cardInstances.Add(cardInstance->nInstanceId, cardInstance); //replace in TMap

	cardInstance->FinalizeSetup();

	if (nCardListLocation == CARD_LIST_LOC_INVALID)
	{
		AddCardInstanceToList(cardInstance, CARD_LIST_LOC_HAND, nPlayer);
	}

	return cardInstance;
}

UCardInstance* UCardManager::ChooseCreatureToExclude(int32 nPlayer)
{
	if (playerDeckDefinitions[nPlayer]->GetDeckFaction() == FactionId_No_Mans_Land)
	{
		TArray<UCardInstance*> allCreaturesNonHeroList;
		int32 nIndex = 0;

		GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nPlayer, allCreaturesNonHeroList);
		GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nPlayer, allCreaturesNonHeroList);
		GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nPlayer, allCreaturesNonHeroList);

		if (allCreaturesNonHeroList.Num() > 0)
		{
			nIndex = FMath::Min(FPlatformMath::FloorToInt(UKismetMathLibrary::RandomFloatInRange(0, 1) * allCreaturesNonHeroList.Num()), (allCreaturesNonHeroList.Num() - 1));
			return allCreaturesNonHeroList[nIndex];
		}
	}
	return nullptr;
}

UCardInstance* UCardManager::GetFirstCardInHandWithEffect(int32 nCardEffect, int32 nPlayer)
{
	int32 nCounter = 0;

	UCardInstance* cardInstance = nullptr;

	TArray<UCardInstance*>& cardsHandList = GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer);

	while (nCounter < cardsHandList.Num())
	{
		cardInstance = cardsHandList[nCounter];

		if (cardInstance->templateRef->HasEffect(nCardEffect))
		{
			return cardInstance;
		}

		nCounter++;
	}

	return nullptr;
}

UCardInstance* UCardManager::GetHigherOrLowerValueTargetCardOnBoard(UCardInstance* cardInstance, int32 nPlayer, bool param3, bool param4, bool param5)
{
	int32 nCounter = 0;

	UCardInstance* highLowCard = nullptr;
	UCardInstance* cardCreature = nullptr;

	TArray<UCardInstance*> creaturesList;
	GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nPlayer, creaturesList);
	GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nPlayer, creaturesList);
	GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nPlayer, creaturesList);

	while (nCounter < creaturesList.Num())
	{
		cardCreature = creaturesList[nCounter];

		if (cardInstance->CanBeCastOn(cardCreature))
		{
			if (highLowCard == nullptr)
			{
				highLowCard = cardCreature;
			}
			else if (IsBetterMatch(cardCreature, highLowCard, nPlayer, param3, param4, param5))
			{
				highLowCard = cardCreature;
			}
		}

		nCounter++;
	}

	return highLowCard;
}

UCardInstance* UCardManager::GetCardInstance(int32 nCardIndex)
{
	return _cardInstances.Contains(nCardIndex) ? _cardInstances[nCardIndex] : nullptr;
}

UCardTemplate* UCardManager::GetCardTemplate(int32 nCardIndex)
{
	return _cardTemplates.Contains(nCardIndex) ? _cardTemplates[nCardIndex] : nullptr;
}

UCardLeaderInstance* UCardManager::GetCardLeader(int32 nPlayer)
{
	TArray<UCardInstance*>& cardLeaderList = GetCardInstanceList(CARD_LIST_LOC_LEADER, nPlayer);
	return cardLeaderList.Num() > 0 ? Cast<UCardLeaderInstance>(cardLeaderList[0]) : nullptr;
}

TArray<UCardInstance*>& UCardManager::GetCardInstanceList(int32 nCardListLocation, int32 nPlayer)
{
	switch (nCardListLocation)
	{
	case CARD_LIST_LOC_DECK:
	{

		LogError("Should not need Deck!!!");


		return *(new TArray<UCardInstance*>());
	}
	case CARD_LIST_LOC_HAND:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListHand[nPlayer];
		}
		break;
	}
	case CARD_LIST_LOC_GRAVEYARD:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListGraveyard[nPlayer];
		}
		break;
	}
	case CARD_LIST_LOC_SIEGE:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListSiege[nPlayer];
		}
		break;
	}
	case CARD_LIST_LOC_RANGED:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListRanged[nPlayer];
		}
		break;
	}
	case CARD_LIST_LOC_MELEE:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListMelee[nPlayer];
		}
		break;
	}
	case CARD_LIST_LOC_SIEGEMODIFIERS:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListSiegeModifier[nPlayer];
		}
		break;
	}
	case CARD_LIST_LOC_RANGEDMODIFIERS:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListRangedModifier[nPlayer];
		}
		break;
	}
	case CARD_LIST_LOC_MELEEMODIFIERS:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListMeleeModifier[nPlayer];
		}
		break;
	}
	case CARD_LIST_LOC_WEATHERSLOT:
	{
		return cardListWeather;
	}
	case CARD_LIST_LOC_LEADER:
	{
		if (nPlayer != PLAYER_INVALID)
		{
			return cardListLeader[nPlayer];
		}
	}
	default:
	{
		break;
	}
	}


	LogError("GFX [ERROR] - CardManager: failed to get card list with player: " + FString::FromInt(nPlayer) + ", and listID: " + FString::FromInt(nCardListLocation));



	return *(new TArray<UCardInstance*>());
}

TArray<UCardInstance*> UCardManager::GetAllCreatures(int32 nPlayer)
{
	TArray<UCardInstance*> allCreatures;

	int32 nIndex = 0;

	TArray<UCardInstance*>& creaturesListMelee = GetCardInstanceList(CARD_LIST_LOC_MELEE, nPlayer);
	nIndex = 0;
	while (nIndex < creaturesListMelee.Num())
	{
		allCreatures.Add(creaturesListMelee[nIndex]);
		++nIndex;
	}

	TArray<UCardInstance*>& creaturesListRanged = GetCardInstanceList(CARD_LIST_LOC_RANGED, nPlayer);
	nIndex = 0;
	while (nIndex < creaturesListRanged.Num())
	{
		allCreatures.Add(creaturesListRanged[nIndex]);
		++nIndex;
	}

	TArray<UCardInstance*>& creaturesListSiege = GetCardInstanceList(CARD_LIST_LOC_SIEGE, nPlayer);
	nIndex = 0;
	while (nIndex < creaturesListSiege.Num())
	{
		allCreatures.Add(creaturesListSiege[nIndex]);
		++nIndex;
	}

	return allCreatures;
}

TArray<UCardInstance*> UCardManager::GetAllCreaturesInHand(int32 nPlayer)
{
	int32 nCounter = 0;

	UCardInstance* cardInstance = nullptr;

	TArray<UCardInstance*> resultList;
	TArray<UCardInstance*>& cardsHandList = GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer);

	while (nCounter < cardsHandList.Num())
	{
		cardInstance = cardsHandList[nCounter];

		if (cardInstance->templateRef->IsType(CardType_Creature))
		{
			resultList.Add(cardInstance);
		}

		nCounter++;
	}

	return resultList;
}

TArray<UCardInstance*> UCardManager::GetCardsInSlotIdWithEffect(int32 nCardEffect, int32 nPlayer, int32 nCardListLocation)
{
	int32 nCounter = 0;

	UCardInstance* cardInstance = nullptr;

	TArray<UCardInstance*> cardsWithEffectList;

	if (nCardListLocation == -1)
	{
		TArray<UCardInstance*>& cardsRowsListMelee = GetCardInstanceList(CARD_LIST_LOC_MELEE, nPlayer);

		nCounter = 0;
		while (nCounter < cardsRowsListMelee.Num())
		{
			cardInstance = cardsRowsListMelee[nCounter];

			if (cardInstance->templateRef->HasEffect(nCardEffect))
			{
				cardsWithEffectList.Add(cardInstance);
			}

			nCounter++;
		}

		TArray<UCardInstance*>& cardsRowsListRanged = GetCardInstanceList(CARD_LIST_LOC_RANGED, nPlayer);

		nCounter = 0;
		while (nCounter < cardsRowsListRanged.Num())
		{
			cardInstance = cardsRowsListRanged[nCounter];

			if (cardInstance->templateRef->HasEffect(nCardEffect))
			{
				cardsWithEffectList.Add(cardInstance);
			}

			nCounter++;
		}

		TArray<UCardInstance*>& cardsRowsListSiege = GetCardInstanceList(CARD_LIST_LOC_SIEGE, nPlayer);

		nCounter = 0;
		while (nCounter < cardsRowsListSiege.Num())
		{
			cardInstance = cardsRowsListSiege[nCounter];

			if (cardInstance->templateRef->HasEffect(nCardEffect))
			{
				cardsWithEffectList.Add(cardInstance);
			}

			nCounter++;
		}
	}
	else
	{
		TArray<UCardInstance*>& cardsRowList = GetCardInstanceList(nCardListLocation, nPlayer);

		nCounter = 0;

		while (nCounter < cardsRowList.Num())
		{
			cardInstance = cardsRowList[nCounter];

			if (cardInstance->templateRef->HasEffect(nCardEffect))
			{
				cardsWithEffectList.Add(cardInstance);
			}

			nCounter++;
		}
	}

	return cardsWithEffectList;
}

TArray<UCardInstance*> UCardManager::GetCardsInHandWithEffect(int32 nCardEffect, int32 nPlayer)
{
	int32 nCounter = 0;

	UCardInstance* cardInstance = nullptr;

	TArray<UCardInstance*> cardsHandWithEffectList;
	TArray<UCardInstance*>& cardsHandList = GetCardInstanceList(CARD_LIST_LOC_HAND, nPlayer);

	while (nCounter < cardsHandList.Num())
	{
		cardInstance = cardsHandList[nCounter];

		if (cardInstance->templateRef->HasEffect(nCardEffect))
		{
			cardsHandWithEffectList.Add(cardInstance);
		}

		nCounter++;
	}

	return cardsHandWithEffectList;
}

TArray<UCardInstance*> UCardManager::GetScorchTargets(int32 nCardType, int32 nPlayer)
{
	int32 nCounter = 0;
	TArray<UCardInstance*> resultList;

	UCardInstance* cardInstance = nullptr;

	int32 nPower = 0;
	int32 nCurrentPlayerID = PLAYER_1;

	while (nCurrentPlayerID < PLAYER_BOTH /*PLAYER_2 + 1*/)
	{
		if (nCurrentPlayerID == nPlayer || nPlayer == PLAYER_BOTH)
		{
			if ((nCardType & CardType_Melee) != CardType_None)
			{
				TArray<UCardInstance*>& cardInstanceListMelee = GetCardInstanceList(CARD_LIST_LOC_MELEE, nCurrentPlayerID);
				nCounter = 0;

				while (nCounter < cardInstanceListMelee.Num())
				{
					cardInstance = cardInstanceListMelee[nCounter];

					if (cardInstance->GetTotalPower() >= nPower
						&& (cardInstance->templateRef->nTypeArray & nCardType) != CardType_None
						&& !cardInstance->templateRef->IsType(CardType_Hero))
					{
						if (cardInstance->GetTotalPower() > nPower)
						{
							nPower = cardInstance->GetTotalPower();
							resultList.Empty();//result.Num() = 0;
							resultList.Add(cardInstance);
						}
						else
						{
							resultList.Add(cardInstance);
						}
					}

					++nCounter;
				}
			}
			if ((nCardType & CardType_Ranged) != CardType_None)
			{
				TArray<UCardInstance*>& cardInstanceListRanged = GetCardInstanceList(CARD_LIST_LOC_RANGED, nCurrentPlayerID);
				nCounter = 0;

				while (nCounter < cardInstanceListRanged.Num())
				{
					cardInstance = cardInstanceListRanged[nCounter];

					if (cardInstance->GetTotalPower() >= nPower
						&& (cardInstance->templateRef->nTypeArray & nCardType) != CardType_None
						&& !cardInstance->templateRef->IsType(CardType_Hero))
					{
						if (cardInstance->GetTotalPower() > nPower)
						{
							nPower = cardInstance->GetTotalPower();
							resultList.Empty();//result.Num() = 0;
							resultList.Add(cardInstance);
						}
						else
						{
							resultList.Add(cardInstance);
						}
					}

					++nCounter;
				}
			}
			if ((nCardType & CardType_Siege) != CardType_None)
			{
				TArray<UCardInstance*>& cardInstanceListSiege = GetCardInstanceList(CARD_LIST_LOC_SIEGE, nCurrentPlayerID);
				nCounter = 0;

				while (nCounter < cardInstanceListSiege.Num())
				{
					cardInstance = cardInstanceListSiege[nCounter];

					if (cardInstance->GetTotalPower() >= nPower
						&& (cardInstance->templateRef->nTypeArray & nCardType) != CardType_None
						&& !cardInstance->templateRef->IsType(CardType_Hero))
					{
						if (cardInstance->GetTotalPower() > nPower)
						{
							nPower = cardInstance->GetTotalPower();
							resultList.Empty();//result.Num() = 0;
							resultList.Add(cardInstance);
						}
						else
						{
							resultList.Add(cardInstance);
						}
					}

					++nCounter;
				}
			}
		}

		++nCurrentPlayerID;
	}

	return resultList;
}

void UCardManager::GetAllCreaturesNonHero(int32 nCardListLocation, int32 nPlayer, TArray<UCardInstance*>& cardList)
{
	int32 nCounter = 0;
	UCardInstance* cardInstance = nullptr;
	TArray<UCardInstance*>& cardInstanceList = GetCardInstanceList(nCardListLocation, nPlayer);

	if (cardInstanceList.Num() == 0)
	{
		LogError("GFX [ERROR] - Failed to get card instance list for list " + ListIDToString(nCardListLocation) + ", and playerIndex " + FString::FromInt(nPlayer));
	}

	while (nCounter < cardInstanceList.Num())
	{
		cardInstance = cardInstanceList[nCounter];

		if (cardInstance->templateRef->IsType(CardType_Creature) && !cardInstance->templateRef->IsType(CardType_Hero))
		{
			cardList.Add(cardInstance);
		}

		nCounter++;
	}
}

void UCardManager::GetRessurectionTargets(int32 nPlayer, TArray<UCardInstance*>& targetsList, bool bUpdate)
{
	UCardInstance* cardInstance = nullptr;
	TArray<UCardInstance*>& graveyardList = GetCardInstanceList(CARD_LIST_LOC_GRAVEYARD, nPlayer);
	int32 nCounter = 0;

	while (nCounter < graveyardList.Num())
	{
		cardInstance = graveyardList[nCounter];

		if (cardInstance->templateRef->IsType(CardType_Creature) && !cardInstance->templateRef->IsType(CardType_Hero))
		{
			if (bUpdate)
			{
				cardInstance->RecalculatePowerPotential(this);
			}

			targetsList.Add(cardInstance);
		}
		nCounter++;
	}
}

FCardAndPointsCombo UCardManager::GetHigherOrLowerValueCardFromTargetGraveyard(int32 nPlayer, bool param2, bool param3, bool param4, bool bEnemy)
{
	FCardAndPointsCombo combo;

	int32 nCounter = 0;
	int32 nCounterNurse = 0;

	int32 nCardSpy = 0;
	int32 nCardGraveSpy = 0;

	int32 nPlayerID;

	int32 finalComboPointsRegular = 0;
	int32 finalComboPointsSpecial = 0;

	UCardInstance* finalCardPoints = nullptr;
	UCardInstance* cardCreature = nullptr;

	UCardInstance* normalCardCandidate = nullptr;
	UCardInstance* cardNurseCandidate = nullptr;
	UCardInstance* cardSpyCandidate = nullptr;
	UCardInstance* cardMeleeScorchCandidate = nullptr;

	UCardInstance* seemsUselessCard = nullptr;

	UCardInstance* cardGraveyardInstance = nullptr;
	UCardInstance* cardForGraveRegular = nullptr;
	UCardInstance* cardForGraveSpy = nullptr;
	UCardInstance* cardForGraveMeleeScorch = nullptr;

	TArray<UCardInstance*> cardGraveyardList;
	TArray<UCardInstance*> cardNurseGraveyardList;
	TArray<UCardInstance*> cardNurseList;

	TArray<UCardInstance*> creaturesNonHeroList;
	GetAllCreaturesNonHero(CARD_LIST_LOC_GRAVEYARD, nPlayer, creaturesNonHeroList);

	nCounter = 0;
	while (nCounter < creaturesNonHeroList.Num())
	{
		cardCreature = creaturesNonHeroList[nCounter];
		if (finalCardPoints == nullptr)
		{
			finalCardPoints = cardCreature;
		}
		if (cardCreature->templateRef->IsType(CardType_Spy))
		{
			if (cardSpyCandidate == nullptr)
			{
				cardSpyCandidate = cardCreature;
			}
			else if (cardSpyCandidate && IsBetterMatchForGrave(cardCreature, cardSpyCandidate, nPlayer, param2, param3, param4))
			{
				cardSpyCandidate = cardCreature;
			}
		}
		else if (cardCreature->templateRef->HasEffect(CardEffect_MeleeScorch))
		{
			cardMeleeScorchCandidate = cardCreature;
		}
		else if (cardCreature->templateRef->HasEffect(CardEffect_Nurse))
		{
			if (cardNurseCandidate == nullptr)
			{
				cardNurseCandidate = cardCreature;
			}
			else if (cardNurseCandidate && IsBetterMatchForGrave(cardCreature, cardNurseCandidate, nPlayer, param2, param3, param4))
			{
				cardNurseCandidate = cardCreature;
			}

			cardNurseList.Add(cardCreature);
		}
		else if (normalCardCandidate == nullptr)
		{
			normalCardCandidate = cardCreature;
		}
		else if (normalCardCandidate && IsBetterMatchForGrave(cardCreature, normalCardCandidate, nPlayer, param2, param3, param4))
		{
			normalCardCandidate = cardCreature;
		}

		nCounter++;
	}

	if (bEnemy && cardNurseList.Num() > 0)
	{
		cardGraveyardList.Empty();
		nPlayerID = nPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1);
		GetAllCreaturesNonHero(CARD_LIST_LOC_GRAVEYARD, nPlayerID, cardGraveyardList);
		cardNurseGraveyardList.Empty();
		nCounter = 0;
		while (nCounter < cardGraveyardList.Num())
		{
			cardGraveyardInstance = cardGraveyardList[nCounter];
			if (seemsUselessCard == nullptr)
			{

				LogError("this card seems useless!!");


				seemsUselessCard = cardGraveyardInstance;
			}
			if (cardGraveyardInstance->templateRef->IsType(CardType_Spy))
			{
				if (cardForGraveSpy == nullptr)
				{
					cardForGraveSpy = cardGraveyardInstance;
				}
				if (cardForGraveSpy && IsBetterMatchForGrave(cardGraveyardInstance, cardForGraveSpy, nPlayerID, param2, param3, param4))
				{
					cardForGraveSpy = cardGraveyardInstance;
				}
			}
			else if (cardGraveyardInstance->templateRef->HasEffect(CardEffect_MeleeScorch))
			{
				cardForGraveMeleeScorch = cardGraveyardInstance;
			}
			else if (cardGraveyardInstance->templateRef->HasEffect(CardEffect_Nurse))
			{
				cardNurseGraveyardList.Add(cardGraveyardInstance);
			}
			else if (cardForGraveRegular == nullptr)
			{
				cardForGraveRegular = cardGraveyardInstance;
			}
			else if (cardForGraveRegular && IsBetterMatchForGrave(cardGraveyardInstance, cardForGraveRegular, nPlayerID, param2, param3, param4))
			{
				cardForGraveRegular = cardGraveyardInstance;
			}
			nCounter++;
		}
		if (cardForGraveSpy)
		{
			finalComboPointsSpecial = FMath::Max(0, 10 - cardForGraveSpy->GetTotalPower());
			nCardGraveSpy = finalComboPointsSpecial;
		}
		else if (cardForGraveMeleeScorch)
		{
			finalComboPointsSpecial = cardForGraveMeleeScorch->GetTotalPower();
		}
		else if (cardForGraveRegular)
		{
			finalComboPointsSpecial = cardForGraveRegular->GetTotalPower();
		}
		if (cardNurseGraveyardList.Num() > 0)
		{
			nCounterNurse = 0;
			while (nCounterNurse < cardNurseGraveyardList.Num())
			{
				finalComboPointsSpecial = finalComboPointsSpecial + cardNurseGraveyardList[nCounterNurse]->GetTotalPower();
				nCounterNurse = nCounterNurse + 1;
			}
		}
		if (cardNurseCandidate)
		{
			finalComboPointsSpecial = finalComboPointsSpecial + cardNurseCandidate->GetTotalPower();
		}
	}
	if (cardSpyCandidate)
	{
		finalComboPointsRegular = FMath::Max(0, 10 - cardSpyCandidate->GetTotalPower());
		nCardSpy = finalComboPointsRegular;
		finalCardPoints = cardSpyCandidate;
	}
	else if (cardMeleeScorchCandidate)
	{
		finalComboPointsRegular = cardMeleeScorchCandidate->GetTotalPower();
		finalCardPoints = cardMeleeScorchCandidate;
	}
	else if (normalCardCandidate)
	{
		finalComboPointsRegular = normalCardCandidate->GetTotalPower();
		finalCardPoints = normalCardCandidate;
	}
	if (!bEnemy && cardNurseList.Num() > 0)
	{
		nCounterNurse = 0;
		while (nCounterNurse < cardNurseList.Num())
		{
			finalComboPointsRegular = finalComboPointsRegular + cardNurseList[nCounterNurse]->GetTotalPower();
			nCounterNurse = nCounterNurse + 1;
		}
	}
	else if (!cardSpyCandidate && !cardMeleeScorchCandidate && !normalCardCandidate && cardNurseCandidate)
	{
		finalComboPointsRegular = cardNurseCandidate->GetTotalPower();
		finalCardPoints = cardNurseCandidate;
	}
	if (bEnemy && cardNurseCandidate)
	{
		if (!nCardGraveSpy && !nCardSpy && finalComboPointsSpecial > finalComboPointsRegular)
		{
			combo.cardInstance = cardNurseCandidate;
			combo.comboPoints = finalComboPointsSpecial;
		}
		else if (!nCardSpy && nCardGraveSpy || nCardGraveSpy > nCardSpy)
		{
			combo.cardInstance = cardNurseCandidate;
			combo.comboPoints = finalComboPointsSpecial;
		}
		else
		{
			combo.cardInstance = finalCardPoints;
			combo.comboPoints = finalComboPointsRegular;
		}
	}
	else
	{
		combo.cardInstance = finalCardPoints;
		combo.comboPoints = finalComboPointsRegular;
	}
	return combo;
}

bool UCardManager::IsBetterMatchForGrave(UCardInstance* cardInstance, UCardInstance* cardCompareTo, int32 nPlayer, bool param4, bool param5, bool param6)
{
	bool bIsBetter = false;

	bool bIsSpy = cardInstance->templateRef->IsType(CardType_Spy);
	bool bIsSpyCompareTo = cardCompareTo->templateRef->IsType(CardType_Spy);
	bool bHasMeleeScorch = cardInstance->templateRef->HasEffect(CardEffect_MeleeScorch);
	bool bHasMeleeScorchCompareTo = cardCompareTo->templateRef->HasEffect(CardEffect_MeleeScorch);
	bool bHasNurse = cardInstance->templateRef->HasEffect(CardEffect_Nurse);
	bool bHasNurseCompareTo = cardCompareTo->templateRef->HasEffect(CardEffect_Nurse);

	int32 nPlayerID = nPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1);
	int32 nScore = CalculatePlayerScore(CARD_LIST_LOC_MELEE, nPlayerID);

	if (param5 || param6)
	{
		bIsBetter = param4 == true ? (false) : (true);
	}
	if (bIsSpy || bIsSpyCompareTo)
	{
		if (!bIsSpyCompareTo)
		{
			return true;
		}
		if (param5 && bIsSpy && CheckIfHigherOrLower(cardInstance, cardCompareTo, bIsBetter))
		{
			return true;
		}
		if (bIsSpy && CheckIfHigherOrLower(cardInstance, cardCompareTo, param4))
		{
			return true;
		}
		return false;
	}
	else if (bHasMeleeScorch || bHasMeleeScorchCompareTo)
	{
		if (bHasMeleeScorchCompareTo)
		{
			return false;
		}
		if (nScore >= 10)
		{
			return true;
		}
		return false;
	}
	else if (bHasNurse || bHasNurseCompareTo)
	{
		if (!bHasNurseCompareTo)
		{
			return true;
		}
		if (param6 && bHasNurse && CheckIfHigherOrLower(cardInstance, cardCompareTo, bIsBetter))
		{
			return true;
		}
		if (bHasNurse && CheckIfHigherOrLower(cardInstance, cardCompareTo, true))
		{
			return true;
		}
		return false;
	}
	else if (CheckIfHigherOrLower(cardInstance, cardCompareTo, param4))
	{
		return true;
	}
	return false;
}

bool UCardManager::IsBetterMatch(UCardInstance* cardInstance, UCardInstance* cardCompareTo, int32 nPlayer, bool param4, bool param5, bool param6)
{
	bool bIsBetter = false;

	bool bIsSpy = cardInstance->templateRef->IsType(CardType_Spy);
	bool bIsSpyCompareTo = cardCompareTo->templateRef->IsType(CardType_Spy);
	bool bIsMeleeScorch = cardInstance->templateRef->HasEffect(CardEffect_MeleeScorch);
	bool bIsMeleeScorchCompareTo = cardCompareTo->templateRef->HasEffect(CardEffect_MeleeScorch);
	bool bIsNurse = cardInstance->templateRef->HasEffect(CardEffect_Nurse);
	bool bIsNurseCompareTo = cardCompareTo->templateRef->HasEffect(CardEffect_Nurse);

	int32 nPlayerID = nPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1);
	int32 nScore = CalculatePlayerScore(CARD_LIST_LOC_MELEE, nPlayerID);

	if (param5 || param6)
	{
		bIsBetter = param4 == true ? (false) : (true);
	}

	if (bIsSpy || bIsSpyCompareTo)
	{
		if (!bIsSpyCompareTo)
		{
			return true;
		}

		if (param5 && bIsSpy && CheckIfHigherOrLower(cardInstance, cardCompareTo, bIsBetter))
		{
			return true;
		}

		if (bIsSpy && CheckIfHigherOrLower(cardInstance, cardCompareTo, param4))
		{
			return true;
		}

		return false;
	}
	else if (bIsMeleeScorch || bIsMeleeScorchCompareTo)
	{
		if (bIsMeleeScorchCompareTo)
		{
			return false;
		}

		if (nScore >= 10)
		{
			return true;
		}

		return false;
	}
	else if (bIsNurse || bIsNurseCompareTo)
	{
		if (!bIsNurseCompareTo)
		{
			return true;
		}

		if (param6 && bIsNurse && CheckIfHigherOrLower(cardInstance, cardCompareTo, bIsBetter))
		{
			return true;
		}

		if (bIsNurse && CheckIfHigherOrLower(cardInstance, cardCompareTo, true))
		{
			return true;
		}

		return false;
	}
	else if (CheckIfHigherOrLower(cardInstance, cardCompareTo, param4))
	{
		return true;
	}
	return false;
}

bool UCardManager::CheckIfHigherOrLower(UCardInstance* cardInstance, UCardInstance* cardCompareTo, bool bHigher)
{
	if (bHigher)
	{
		if (cardInstance->GetTotalPower() > cardCompareTo->GetTotalPower())
		{
			return true;
		}
		return false;
	}
	else
	{
		if (cardInstance->GetTotalPower() < cardCompareTo->GetTotalPower())
		{
			return true;
		}
		return false;
	}
}

bool UCardManager::TryDrawAndPlaySpecificCard_Weather(int32 nPlayer, int32 nCardIndex)
{
	UCardInstance* cardInstance = nullptr;

	UCardsDeck* cardsDeck = playerDeckDefinitions[nPlayer];

	if (cardsDeck->TryDrawSpecificCard(nCardIndex))
	{
		cardInstance = SpawnCardInstance(nCardIndex, nPlayer);

		AddCardInstanceToList(cardInstance, CARD_LIST_LOC_WEATHERSLOT, PLAYER_INVALID);


		if (cardInstance)
			LogWarning("GFX - Player " + FString::FromInt(nPlayer) + " drew the following Card: " + _cardTemplates[cardInstance->nTemplateId]->sTitle);
		else LogError("TryDrawAndPlaySpecificCard_Weather | No card!!!");



		return true;
	}
	return false;
}

bool UCardManager::TryDrawSpecificCard(int32 nPlayer, int32 nCardIndex)
{
	UCardInstance* cardInstance = nullptr;

	UCardsDeck* cardsDeck = playerDeckDefinitions[nPlayer];

	if (cardsDeck->TryDrawSpecificCard(nCardIndex))
	{
		cardInstance = SpawnCardInstance(nCardIndex, nPlayer);
		AddCardInstanceToList(cardInstance, CARD_LIST_LOC_HAND, nPlayer);


		if (cardInstance)
			LogWarning("GFX - Player " + FString::FromInt(nPlayer) + " drew the following Card: " + _cardTemplates[cardInstance->nTemplateId]->sTitle);
		else LogError("TryDrawSpecificCard | No card!!!");



		return true;
	}

	return false;
}

bool UCardManager::DrawCards(int32 nPlayer, int32 nCardCounter)
{
	//TODO sound
	int32 nCounter = 0;

	while (nCounter < nCardCounter)
	{
		if (!DrawCard(nPlayer))
		{
			return false;
		}

		nCounter++;
	}

	return true;
}

bool UCardManager::DrawCard(int32 nPlayer)
{
	int32 nCardIndex = 0;

	UCardInstance* cardInstance = nullptr;

	UCardsDeck* cardsDeck = playerDeckDefinitions[nPlayer];

	if (cardsDeck->cardIndicesInDeck.Num() > 0)
	{
		nCardIndex = cardsDeck->DrawCard();

		cardInstance = SpawnCardInstance(nCardIndex, nPlayer);

		AddCardInstanceToList(cardInstance, CARD_LIST_LOC_HAND, nPlayer);

		//TODO sound

		if (cardInstance)
			LogWarning("GFX - Player " + FString::FromInt(nPlayer) + " drew the following Card: " + _cardTemplates[cardInstance->nTemplateId]->sTitle);
		else LogError("DrawCard | No card!!!");

		return true;
	}
	else
	{
		LogWarning("GFX - Player " + FString::FromInt(nPlayer) + " has no more cards to draw!");
	}

	return false;
}

bool UCardManager::SummonFromDeck(int32 nPlayer, int32 nCardIndex)
{
	bool bSummon = false;
	UCardsDeck* playerDeck = playerDeckDefinitions[nPlayer];
	UCardInstance* cardInstance = nullptr;

	while (playerDeck->TryDrawSpecificCard(nCardIndex))
	{
		bSummon = true;

		cardInstance = SpawnCardInstance(nCardIndex, nPlayer);

		//TODO play summon fx
		/*cardInstance->playSummonedFX = true;*/

		if (cardInstance->templateRef->IsType(CardType_Melee))
		{
			AddCardInstanceToList(cardInstance, CARD_LIST_LOC_MELEE, nPlayer);
			continue;
		}

		if (cardInstance->templateRef->IsType(CardType_Ranged))
		{
			AddCardInstanceToList(cardInstance, CARD_LIST_LOC_RANGED, nPlayer);
			continue;
		}

		if (cardInstance->templateRef->IsType(CardType_Siege))
		{
			AddCardInstanceToList(cardInstance, CARD_LIST_LOC_SIEGE, nPlayer);
			continue;
		}
	}

	return bSummon;
}

void UCardManager::InitializeLists()
{
	UCardsDeck* deckP1 = NewObject<UCardsDeck>();
	UCardsDeck* deckP2 = NewObject<UCardsDeck>();

	deckP1->AddToRoot();
	deckP2->AddToRoot();

	playerDeckDefinitions.Add(deckP1);
	playerDeckDefinitions.Add(deckP2);

	currentPlayerScores.Add(0);
	currentPlayerScores.Add(0);

	TArray<UCardInstance*> cardListHandP1;
	TArray<UCardInstance*> cardListHandP2;
	cardListHand.Add(PLAYER_1, cardListHandP1);
	cardListHand.Add(PLAYER_2, cardListHandP2);

	TArray<UCardInstance*> cardListGraveyardP1;
	TArray<UCardInstance*> cardListGraveyardP2;
	cardListGraveyard.Add(PLAYER_1, cardListGraveyardP1);
	cardListGraveyard.Add(PLAYER_2, cardListGraveyardP2);

	TArray<UCardInstance*> cardListSiegeP1;
	TArray<UCardInstance*> cardListSiegeP2;
	cardListSiege.Add(PLAYER_1, cardListSiegeP1);
	cardListSiege.Add(PLAYER_2, cardListSiegeP2);

	TArray<UCardInstance*> cardListRangedP1;
	TArray<UCardInstance*> cardListRangedP2;
	cardListRanged.Add(PLAYER_1, cardListRangedP1);
	cardListRanged.Add(PLAYER_2, cardListRangedP2);

	TArray<UCardInstance*> cardListMeleeP1;
	TArray<UCardInstance*> cardListMeleeP2;
	cardListMelee.Add(PLAYER_1, cardListMeleeP1);
	cardListMelee.Add(PLAYER_2, cardListMeleeP2);

	TArray<UCardInstance*> cardListSiegeModifierP1;
	TArray<UCardInstance*> cardListSiegeModifierP2;
	cardListSiegeModifier.Add(PLAYER_1, cardListSiegeModifierP1);
	cardListSiegeModifier.Add(PLAYER_2, cardListSiegeModifierP2);

	TArray<UCardInstance*> cardListRangedModifierP1;
	TArray<UCardInstance*> cardListRangedModifierP2;
	cardListRangedModifier.Add(PLAYER_1, cardListRangedModifierP1);
	cardListRangedModifier.Add(PLAYER_2, cardListRangedModifierP2);

	TArray<UCardInstance*> cardListMeleeModifierP1;
	TArray<UCardInstance*> cardListMeleeModifierP2;
	cardListMeleeModifier.Add(PLAYER_1, cardListMeleeModifierP1);
	cardListMeleeModifier.Add(PLAYER_2, cardListMeleeModifierP2);

	TArray<UCardInstance*> cardListLeaderP1;
	TArray<UCardInstance*> cardListLeaderP2;
	cardListLeader.Add(PLAYER_1, cardListLeaderP1);
	cardListLeader.Add(PLAYER_2, cardListLeaderP2);

	TArray<UCardInstance*> cardListWeatherBoth;
	cardListWeather = cardListWeatherBoth;

	roundResults.Empty();
	UCardsRoundResult* round0 = NewObject<UCardsRoundResult>();
	UCardsRoundResult* round1 = NewObject<UCardsRoundResult>();
	UCardsRoundResult* round2 = NewObject<UCardsRoundResult>();
	round0->AddToRoot();
	round1->AddToRoot();
	round2->AddToRoot();
	roundResults.Add(round0);
	roundResults.Add(round1);
	roundResults.Add(round2);
}

FString UCardManager::ListIDToString(int32 nList)
{
	switch (nList)
	{
	case CARD_LIST_LOC_DECK:
	{
		return "DECK";
	}
	case CARD_LIST_LOC_HAND:
	{
		return "HAND";
	}
	case CARD_LIST_LOC_GRAVEYARD:
	{
		return "GRAVEYARD";
	}
	case CARD_LIST_LOC_SIEGE:
	{
		return "SIEGE";
	}
	case CARD_LIST_LOC_RANGED:
	{
		return "RANGED";
	}
	case CARD_LIST_LOC_MELEE:
	{
		return "MELEE";
	}
	case CARD_LIST_LOC_SIEGEMODIFIERS:
	{
		return "SIEGEMODIFIERS";
	}
	case CARD_LIST_LOC_RANGEDMODIFIERS:
	{
		return "RANGEDMODIFIERS";
	}
	case CARD_LIST_LOC_MELEEMODIFIERS:
	{
		return "MELEEMODIFIERS";
	}
	case CARD_LIST_LOC_WEATHERSLOT:
	{
		return "WEATHER";
	}
	case CARD_LIST_LOC_LEADER:
	{
		return "LEADER";
	}
	case CARD_LIST_LOC_INVALID:
	default:
	{
		return "INVALID";
	}
	}

	return "INVALID";
}

int32 UCardManager::GetWinningPlayer()
{
	if (currentPlayerScores[PLAYER_1] > currentPlayerScores[PLAYER_2])
	{
		return PLAYER_1;
	}
	if (currentPlayerScores[PLAYER_1] < currentPlayerScores[PLAYER_2])
	{
		return PLAYER_2;
	}
	return PLAYER_INVALID;
}

int32 UCardManager::CalculatePlayerScore(int32 nCardListLocation, int32 nPlayer)
{
	int32 nCounter = 0;

	TArray<UCardInstance*>& cardInstanceList = GetCardInstanceList(nCardListLocation, nPlayer);

	int32 nScore = 0;

	while (nCounter < cardInstanceList.Num())
	{
		nScore = nScore + cardInstanceList[nCounter]->GetTotalPower();
		++nCounter;
	}

	return nScore;
}

void UCardManager::TraceRoundResults()
{

	int32 nCounter = 0;
	LogWarning("GFX -------------------------------- START TRACE ROUND RESULTS ----------------------------------");
	LogWarning("GFX =============================================================================================");
	if (roundResults.Num() > 0)
	{
		nCounter = 0;
		while (nCounter < roundResults.Num())
		{
			LogWarning("GFX - " + roundResults[nCounter]->ToString());
			++nCounter;
		}
	}
	else
	{
		LogWarning("GFX -------------- Round Results is empty!!! -------------");
	}
	LogWarning("GFX =============================================================================================");
	LogWarning("GFX ---------------------------------- END TRACE ROUND RESULTS ----------------------------------");

}

void UCardManager::Reset()
{
	//seems end game reset, not round reset?

	//SendToGraveyard will clear the CardSlots
	//GetBoardRenderer()->ClearAllCards();

	_cardInstances.Empty();

	cardListHand[0].Empty();
	cardListHand[1].Empty();

	cardListGraveyard[0].Empty();
	cardListGraveyard[1].Empty();

	cardListSiege[0].Empty();
	cardListSiege[1].Empty();

	cardListRanged[0].Empty();
	cardListRanged[1].Empty();

	cardListMelee[0].Empty();
	cardListMelee[1].Empty();

	cardListSiegeModifier[0].Empty();
	cardListSiegeModifier[1].Empty();

	cardListRangedModifier[0].Empty();
	cardListRangedModifier[1].Empty();

	cardListMeleeModifier[0].Empty();
	cardListMeleeModifier[1].Empty();

	cardListLeader[0].Empty();
	cardListLeader[1].Empty();

	cardListWeather.Empty();

	roundResults[0]->Reset();
	roundResults[1]->Reset();
	roundResults[2]->Reset();

	//TODO GetBoardRenderer() update text (lives, score, etc.)
	//reset the Passed message as well
	//GetBoardRenderer()->ResetStuff();

	cardEffectManager->FlushAllEffects();

	RecalculateScores();
}