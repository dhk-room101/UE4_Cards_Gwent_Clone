// Fill out your copyright notice in the Description page of Project Settings.

#include "CardInstance.h"

#include "CardTransaction.h"
#include "CardManager.h"
#include "CardEffectManager.h"
#include "CardSlot.h"

#include "CardsCardValues.h"
#include "CardsGameMain.h"
#include "CardsGameFlowController.h"
#include "CardsDeck.h"

#include "CardsPlayerController.h"
#include "AIPlayerController.h"

#include "UMG.h"

#include "ldf.h"

#include "Kismet/KismetMathLibrary.h"

UCardInstance::UCardInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	nOwningPlayer = PLAYER_INVALID;
	nInList = CARD_LIST_LOC_INVALID;
	nListsPlayer = PLAYER_INVALID;
	nLastListApplied = CARD_LIST_LOC_INVALID;
	nLastListPlayerApplied = PLAYER_INVALID;

	_lastCalculatedPowerPotential = NewObject<UCardTransaction>();
}

void UCardInstance::FinalizeSetup()
{

}

void UCardInstance::RecalculatePowerPotential(UCardManager* cardManager)
{
	TArray<UCardInstance*>& cardsHandList = cardManager->GetCardInstanceList(CARD_LIST_LOC_HAND, nListsPlayer);

	TArray<UCardInstance*> cardsListWeather;
	TArray<UCardInstance*> graveyardList;
	TArray<UCardInstance*> scorchTargetsList;
	TArray<UCardInstance*> scorchTargetsOpponentList;

	int32 nPlayerID = nListsPlayer != PLAYER_1 ? PLAYER_1 : PLAYER_2;
	int32 nCounter = 0;
	int32 nSummonFlag = 0;
	int32 nPowerChangeResult = 0;
	int32 nPowerChangeResultAndNurseValue = 0;
	int32 nTotalPowerMeleeRange = 0;
	int32 nTotalPowerImproveNeighbours = 0;
	int32 nTotalPowerMeleeRangeAndCards = 0;
	int32 nTotalPowerImproveNeighboursAndCards = 0;
	int32 nTotalPowerMoralAndImprove = 0;
	int32 nTotalPowerClearSky = 0;
	int32 nTotalPowerBoardCards = 0;
	int32 nTotalPowerAfterModifierMelee = 0;
	int32 nTotalPowerAfterModifierRanged = 0;
	int32 nTotalPowerAfterModifierSiege = 0;
	int32 nTotalPowerBeforeModifier = 0;
	int32 nTotalPowerScorchOpponentAccumulate = 0;
	int32 nTotalPowerScorchOpponent = 0;

	bool bNeedsResurrection = false;

	_lastCalculatedPowerPotential->powerChangeResult = 0;
	_lastCalculatedPowerPotential->strategicValue = 0;
	_lastCalculatedPowerPotential->sourceCardInstanceRef = this;

	if (templateRef->IsType(CardType_Creature))
	{
		_lastCalculatedPowerPotential->targetPlayerID = templateRef->IsType(CardType_Spy) ? nPlayerID : nListsPlayer;
		if (templateRef->IsType(CardType_Melee))
		{
			_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_MELEE;
		}
		else if (templateRef->IsType(CardType_Ranged))
		{
			_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_RANGED;
		}
		else
		{
			_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_SIEGE;
		}

		TArray<UCardInstance*> cardsListCreature = cardManager->cardEffectManager->GetEffectsForList(_lastCalculatedPowerPotential->targetSlotID, nListsPlayer);

		nCounter = 0;
		while (nCounter < cardsListCreature.Num())
		{

			if (cardsListCreature[nCounter] != this)
			{
				effectedByCardsRefList.Add(cardsListCreature[nCounter]);
			}

			++nCounter;
		}

		nTotalPowerMeleeRange = GetTotalPower();
		effectedByCardsRefList.Empty();

		if (templateRef->IsType(CardType_RangedMelee))
		{
			cardsListCreature = cardManager->cardEffectManager->GetEffectsForList(CARD_LIST_LOC_RANGED, nListsPlayer);
			nCounter = 0;
			while (nCounter < cardsListCreature.Num())
			{
				if (cardsListCreature[nCounter] != this)
				{
					effectedByCardsRefList.Add(cardsListCreature[nCounter]);
				}
				++nCounter;
			}

			nTotalPowerImproveNeighbours = GetTotalPower();
			effectedByCardsRefList.Empty();
			if (templateRef->HasEffect(CardEffect_ImproveNeighbours))
			{
				cardsListWeather.Empty();

				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, PLAYER_1, cardsListWeather);
				nTotalPowerMeleeRangeAndCards = nTotalPowerMeleeRange + cardsListWeather.Num();

				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, PLAYER_1, cardsListWeather);
				nTotalPowerImproveNeighboursAndCards = nTotalPowerImproveNeighbours + cardsListWeather.Num();

				if (nTotalPowerImproveNeighboursAndCards > nTotalPowerMeleeRangeAndCards || nTotalPowerImproveNeighboursAndCards == nTotalPowerMeleeRangeAndCards && UKismetMathLibrary::RandomFloatInRange(0, 1) < 0.5)
				{
					nTotalPowerMeleeRange = nTotalPowerImproveNeighbours;
					_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_RANGED;
				}
			}
			else if (nTotalPowerImproveNeighbours > nTotalPowerMeleeRange || nTotalPowerImproveNeighbours == nTotalPowerMeleeRange && UKismetMathLibrary::RandomFloatInRange(0, 1) < 0.5)
			{
				nTotalPowerMeleeRange = nTotalPowerImproveNeighbours;
				_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_RANGED;
			}
		}
		if (templateRef->HasEffect(CardEffect_SameTypeMorale) || templateRef->HasEffect(CardEffect_ImproveNeighbours))
		{
			cardsListCreature.Empty();

			if (_lastCalculatedPowerPotential->targetSlotID == CardType_Melee)
			{
				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nListsPlayer, cardsListCreature);
			}

			if (_lastCalculatedPowerPotential->targetSlotID == CardType_Ranged)
			{
				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nListsPlayer, cardsListCreature);
			}

			if (_lastCalculatedPowerPotential->targetSlotID == CardType_Siege)
			{
				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nListsPlayer, cardsListCreature);
			}

			if (templateRef->HasEffect(CardEffect_ImproveNeighbours))
			{
				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + cardsListCreature.Num();
			}
			else
			{
				nCounter = 0;
				while (nCounter < cardsListCreature.Num())
				{

					if (cardsListCreature[nCounter]->nTemplateId == nTemplateId)
					{
						nTotalPowerMoralAndImprove = cardsListCreature[nCounter]->GetTotalPower();
						cardsListCreature[nCounter]->effectedByCardsRefList.Add(this);
						_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + (cardsListCreature[nCounter]->GetTotalPower() - nTotalPowerMoralAndImprove);
						cardsListCreature[nCounter]->effectedByCardsRefList.Pop();
					}

					++nCounter;
				}
			}
		}
		if (templateRef->HasEffect(CardEffect_SummonClones))
		{
			nSummonFlag = 0;

			cardsListCreature = cardManager->GetCardInstanceList(CARD_LIST_LOC_HAND, nListsPlayer);

			nCounter = 0;
			while (nCounter < cardsListCreature.Num())
			{
				if (templateRef->summonFlags.IndexOfByKey(cardsListCreature[nCounter]->nTemplateId) != -1)
				{
					++nSummonFlag;
				}

				++nCounter;
			}

			nCounter = 0;
			while (nCounter < templateRef->summonFlags.Num())
			{
				nSummonFlag = nSummonFlag + cardManager->playerDeckDefinitions[nListsPlayer]->NumCopiesLeft(templateRef->summonFlags[nCounter]);
				++nCounter;
			}

			_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + nSummonFlag * nTotalPowerMeleeRange;
		}
		if (templateRef->IsType(CardType_Spy))
		{
			_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult - nTotalPowerMeleeRange;
		}
		else
		{
			_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + nTotalPowerMeleeRange;
		}
	}

	if (templateRef->IsType(CardType_Weather))
	{
		nTotalPowerClearSky = 0;
		nTotalPowerBoardCards = 0;

		cardsListWeather.Empty();

		if (templateRef->HasEffect(CardEffect_ClearSky))
		{
			cardsListWeather.Empty();

			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nListsPlayer, cardsListWeather);
			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nListsPlayer, cardsListWeather);
			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nListsPlayer, cardsListWeather);

			nCounter = 0;
			while (nCounter < cardsListWeather.Num())
			{
				nTotalPowerClearSky = nTotalPowerClearSky + (cardsListWeather[nCounter]->GetTotalPower(true) - cardsListWeather[nCounter]->GetTotalPower());
				++nCounter;
			}

			cardsListWeather.Empty();

			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nPlayerID, cardsListWeather);
			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nPlayerID, cardsListWeather);
			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nPlayerID, cardsListWeather);

			nCounter = 0;
			while (nCounter < cardsListWeather.Num())
			{
				nTotalPowerClearSky = nTotalPowerClearSky - (cardsListWeather[nCounter]->GetTotalPower(true) - cardsListWeather[nCounter]->GetTotalPower());
				++nCounter;
			}
		}
		else
		{
			if (templateRef->HasEffect(CardEffect_Melee))
			{
				cardsListWeather.Empty();

				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nListsPlayer, cardsListWeather);

				nCounter = 0;
				while (nCounter < cardsListWeather.Num())
				{

					nTotalPowerBoardCards = cardsListWeather[nCounter]->GetTotalPower();
					cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
					nTotalPowerClearSky = nTotalPowerClearSky + (cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBoardCards);
					cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

					++nCounter;
				}

				cardsListWeather.Empty();

				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nPlayerID, cardsListWeather);

				nCounter = 0;
				while (nCounter < cardsListWeather.Num())
				{

					nTotalPowerBoardCards = cardsListWeather[nCounter]->GetTotalPower();
					cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
					nTotalPowerClearSky = nTotalPowerClearSky - (cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBoardCards);
					cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

					++nCounter;
				}
			}
			if (templateRef->HasEffect(CardEffect_Ranged))
			{
				cardsListWeather.Empty();

				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nListsPlayer, cardsListWeather);

				nCounter = 0;
				while (nCounter < cardsListWeather.Num())
				{

					nTotalPowerBoardCards = cardsListWeather[nCounter]->GetTotalPower();
					cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
					nTotalPowerClearSky = nTotalPowerClearSky + (cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBoardCards);
					cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

					++nCounter;
				}

				cardsListWeather.Empty();

				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nPlayerID, cardsListWeather);

				nCounter = 0;
				while (nCounter < cardsListWeather.Num())
				{

					nTotalPowerBoardCards = cardsListWeather[nCounter]->GetTotalPower();
					cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
					nTotalPowerClearSky = nTotalPowerClearSky - (cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBoardCards);
					cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

					++nCounter;
				}
			}
			if (templateRef->HasEffect(CardEffect_Siege))
			{
				cardsListWeather.Empty();

				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nListsPlayer, cardsListWeather);

				nCounter = 0;
				while (nCounter < cardsListWeather.Num())
				{

					nTotalPowerBoardCards = cardsListWeather[nCounter]->GetTotalPower();
					cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
					nTotalPowerClearSky = nTotalPowerClearSky + (cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBoardCards);
					cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

					++nCounter;
				}

				cardsListWeather.Empty();

				cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nPlayerID, cardsListWeather);

				nCounter = 0;
				while (nCounter < cardsListWeather.Num())
				{

					nTotalPowerBoardCards = cardsListWeather[nCounter]->GetTotalPower();
					cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
					nTotalPowerClearSky = nTotalPowerClearSky - (cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBoardCards);
					cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

					++nCounter;
				}
			}
		}

		_lastCalculatedPowerPotential->powerChangeResult = nTotalPowerClearSky;
		_lastCalculatedPowerPotential->strategicValue = FMath::Max(0, FMath::FloorToInt(cardManager->cardValues->weatherCardValue) - nTotalPowerClearSky);

		if (templateRef->HasEffect(CardEffect_ClearSky))
		{
			_lastCalculatedPowerPotential->strategicValue = FMath::Max(FMath::FloorToInt(_lastCalculatedPowerPotential->strategicValue), 8);
		}

		_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_WEATHERSLOT;
		_lastCalculatedPowerPotential->targetPlayerID = PLAYER_INVALID;
	}

	if (templateRef->HasEffect(CardEffect_Scorch))
	{
		scorchTargetsList = cardManager->GetScorchTargets();
	}

	if (scorchTargetsList.Num() > 0)
	{
		nCounter = 0;
		while (nCounter < scorchTargetsList.Num())
		{
			if (scorchTargetsList[nCounter]->nListsPlayer != nListsPlayer)
			{
				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + scorchTargetsList[nCounter]->GetTotalPower();
			}
			else
			{
				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult - scorchTargetsList[nCounter]->GetTotalPower();
			}

			++nCounter;
		}

		if (_lastCalculatedPowerPotential->powerChangeResult < 0)
		{
			_lastCalculatedPowerPotential->strategicValue = -1;
		}
		else
		{
			_lastCalculatedPowerPotential->strategicValue = FMath::Max(FMath::FloorToInt(templateRef->GetBonusValue()), FMath::FloorToInt(_lastCalculatedPowerPotential->powerChangeResult));
		}


		LogWarning("scorchTargetsList.Num() > 0, returning...");

		return;
	}

	if (templateRef->HasEffect(CardEffect_UnsummonDummy))
	{
		_lastCalculatedPowerPotential->targetCardInstanceRef = cardManager->GetHigherOrLowerValueTargetCardOnBoard(this, nListsPlayer, false, false, true);

		if (_lastCalculatedPowerPotential->targetCardInstanceRef)
		{
			if (_lastCalculatedPowerPotential->targetCardInstanceRef->templateRef->IsType(CardType_Spy))
			{
				_lastCalculatedPowerPotential->powerChangeResult = 0;
			}
			else
			{
				_lastCalculatedPowerPotential->powerChangeResult = -_lastCalculatedPowerPotential->targetCardInstanceRef->GetTotalPower();
			}

			if (cardManager->cardValues->unsummonCardValue + _lastCalculatedPowerPotential->powerChangeResult >= 0)
			{
				_lastCalculatedPowerPotential->strategicValue = FMath::Abs(_lastCalculatedPowerPotential->powerChangeResult);
			}
			else
			{
				_lastCalculatedPowerPotential->strategicValue = cardManager->cardValues->unsummonCardValue + FMath::Abs(_lastCalculatedPowerPotential->powerChangeResult);
			}
		}
		else
		{
			_lastCalculatedPowerPotential->powerChangeResult = -1000;
			_lastCalculatedPowerPotential->strategicValue = -1;
		}
	}

	if (templateRef->IsType(CardType_Row_Modifier) && templateRef->HasEffect(CardEffect_Horn))
	{
		nTotalPowerBeforeModifier = 0;

		nTotalPowerAfterModifierMelee = -1;
		nTotalPowerAfterModifierRanged = -1;
		nTotalPowerAfterModifierSiege = -1;

		cardsListWeather.Empty();

		if (cardManager->GetCardInstanceList(CARD_LIST_LOC_MELEEMODIFIERS, nListsPlayer).Num() == 0)
		{
			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nListsPlayer, cardsListWeather);

			nCounter = 0;
			while (nCounter < cardsListWeather.Num())
			{
				nTotalPowerBeforeModifier = cardsListWeather[nCounter]->GetTotalPower();
				cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
				nTotalPowerAfterModifierMelee = cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBeforeModifier;
				cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

				++nCounter;
			}
		}

		cardsListWeather.Empty();

		if (cardManager->GetCardInstanceList(CARD_LIST_LOC_RANGEDMODIFIERS, nListsPlayer).Num() == 0)
		{
			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nListsPlayer, cardsListWeather);

			nCounter = 0;
			while (nCounter < cardsListWeather.Num())
			{
				nTotalPowerBeforeModifier = cardsListWeather[nCounter]->GetTotalPower();
				cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
				nTotalPowerAfterModifierRanged = cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBeforeModifier;
				cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

				++nCounter;
			}
		}

		cardsListWeather.Empty();

		if (cardManager->GetCardInstanceList(CARD_LIST_LOC_SIEGEMODIFIERS, nListsPlayer).Num() == 0)
		{
			cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nListsPlayer, cardsListWeather);

			nCounter = 0;
			while (nCounter < cardsListWeather.Num())
			{

				nTotalPowerBeforeModifier = cardsListWeather[nCounter]->GetTotalPower();//DHK
				cardsListWeather[nCounter]->effectedByCardsRefList.Add(this);
				nTotalPowerAfterModifierSiege = cardsListWeather[nCounter]->GetTotalPower() - nTotalPowerBeforeModifier;
				cardsListWeather[nCounter]->effectedByCardsRefList.Pop();

				++nCounter;
			}
		}

		if (nTotalPowerAfterModifierSiege == -1 && nTotalPowerAfterModifierMelee == -1 && nTotalPowerAfterModifierRanged == -1)
		{
			_lastCalculatedPowerPotential->powerChangeResult = -1;
			_lastCalculatedPowerPotential->strategicValue = -1;


			LogWarning("all modifiers are -1, returning...");

			return;
		}

		if (nTotalPowerAfterModifierMelee > nTotalPowerAfterModifierSiege && nTotalPowerAfterModifierMelee > nTotalPowerAfterModifierRanged)
		{
			_lastCalculatedPowerPotential->powerChangeResult = nTotalPowerAfterModifierMelee;
			_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_MELEEMODIFIERS;
			_lastCalculatedPowerPotential->targetPlayerID = nListsPlayer;
		}
		else if (nTotalPowerAfterModifierRanged > nTotalPowerAfterModifierSiege)
		{
			_lastCalculatedPowerPotential->powerChangeResult = nTotalPowerAfterModifierRanged;
			_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_RANGEDMODIFIERS;
			_lastCalculatedPowerPotential->targetPlayerID = nListsPlayer;
		}
		else
		{
			_lastCalculatedPowerPotential->powerChangeResult = nTotalPowerAfterModifierSiege;
			_lastCalculatedPowerPotential->targetSlotID = CARD_LIST_LOC_SIEGEMODIFIERS;
			_lastCalculatedPowerPotential->targetPlayerID = nListsPlayer;
		}

		if (_lastCalculatedPowerPotential->powerChangeResult > cardManager->cardValues->hornCardValue)
		{
			_lastCalculatedPowerPotential->strategicValue = FMath::Max(0, FMath::FloorToInt(cardManager->cardValues->hornCardValue * 2 - _lastCalculatedPowerPotential->powerChangeResult));
		}
		else
		{
			_lastCalculatedPowerPotential->strategicValue = cardManager->cardValues->hornCardValue;
		}
	}

	if (templateRef->HasEffect(CardEffect_MeleeScorch))
	{
		scorchTargetsOpponentList.Empty();
		scorchTargetsOpponentList = cardManager->GetScorchTargets(CardType_Melee, GetNotListPlayer());

		if (scorchTargetsOpponentList.Num() > 0 && cardManager->CalculatePlayerScore(CARD_LIST_LOC_MELEE, GetNotListPlayer()) >= 10)
		{
			nTotalPowerScorchOpponentAccumulate = 0;
			nTotalPowerScorchOpponent = 0;

			nCounter = 0;
			while (nCounter < scorchTargetsOpponentList.Num())
			{
				nTotalPowerScorchOpponent = scorchTargetsOpponentList[nCounter]->GetTotalPower();
				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + nTotalPowerScorchOpponent;
				nTotalPowerScorchOpponentAccumulate = nTotalPowerScorchOpponentAccumulate + nTotalPowerScorchOpponent;

				++nCounter;
			}

			if (UKismetMathLibrary::RandomFloatInRange(0, 1) >= 2 / scorchTargetsOpponentList.Num()
				|| UKismetMathLibrary::RandomFloatInRange(0, 1) >= 4 / nTotalPowerScorchOpponentAccumulate)
			{
				_lastCalculatedPowerPotential->strategicValue = 1;
			}
			else
			{
				_lastCalculatedPowerPotential->strategicValue = _lastCalculatedPowerPotential->powerChangeResult;
			}
		}
		else
		{
			_lastCalculatedPowerPotential->strategicValue = _lastCalculatedPowerPotential->powerChangeResult + cardManager->cardValues->scorchCardValue;
		}
	}

	if (templateRef->IsType(CardType_Creature))
	{
		if (templateRef->HasEffect(CardEffect_Nurse))
		{
			graveyardList.Empty();

			bNeedsResurrection = true;

			nCounter = 0;
			while (nCounter < cardsHandList.Num())
			{
				if (!cardsHandList[nCounter]->templateRef->HasEffect(CardEffect_Nurse))
				{
					bNeedsResurrection = false;
					break;
				}
				++nCounter;
			}

			cardManager->GetRessurectionTargets(nListsPlayer, graveyardList, false);

			if (graveyardList.Num() != 0)
			{
				nCounter = 0;
				while (nCounter < graveyardList.Num())
				{
					if (!graveyardList[nCounter]->templateRef->HasEffect(CardEffect_Nurse))
					{
						graveyardList[nCounter]->RecalculatePowerPotential(cardManager);
					}

					++nCounter;
				}


				LogWarning("Doublecheck sorting CardInstance");

				graveyardList.Sort(UCardInstance::PowerChangeSorter);

				nPowerChangeResult = graveyardList[(graveyardList.Num() - 1)]->GetOptimalTransaction()->powerChangeResult;

				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + nPowerChangeResult;

				if (UKismetMathLibrary::RandomFloatInRange(0, 1) <= 1 / cardsHandList.Num()
					|| UKismetMathLibrary::RandomFloatInRange(0, 1) >= 8 / nPowerChangeResult)
				{
					_lastCalculatedPowerPotential->strategicValue = 0;
				}
				else
				{
					nPowerChangeResultAndNurseValue = cardManager->cardValues->nurseCardValue + nPowerChangeResult;
					_lastCalculatedPowerPotential->strategicValue = FMath::Max(nPowerChangeResultAndNurseValue, templateRef->nPower);
				}
			}
			else if (!bNeedsResurrection)
			{
				_lastCalculatedPowerPotential->powerChangeResult = -1000;
				_lastCalculatedPowerPotential->strategicValue = -1;
			}
		}
		else if (_lastCalculatedPowerPotential->strategicValue == 0)
		{
			_lastCalculatedPowerPotential->strategicValue = _lastCalculatedPowerPotential->strategicValue + templateRef->nPower;
		}
	}
}

void UCardInstance::OnFinishedMovingIntoHolder(int32 nCardHolder, int32 nPlayer)
{
	if (GetCurrentPlayerController()->IsDeckBuilder)
	{
		return;
	}

	int32 nCounter = 0;
	TArray<UCardInstance*> cardsWeatherList;

	//TODO CardFXManager
	//UCardFXManager* cardFXManager = nullptr;

	if (nLastListApplied != nCardHolder || nLastListPlayerApplied != nPlayer)
	{
		LogWarning("GFX - finished Moving into holder:" + FString::FromInt(nCardHolder) + ", playerID:" + FString::FromInt(nPlayer) + ", for cardInstance:" + this->templateRef->sTitle);

		nLastListApplied = nCardHolder;
		nLastListPlayerApplied = nPlayer;

		if (nCardHolder == CARD_LIST_LOC_DECK || nCardHolder == CARD_LIST_LOC_LEADER)
		{
			return;
		}

		if (nCardHolder == CARD_LIST_LOC_GRAVEYARD || nCardHolder == CARD_LIST_LOC_HAND)
		{
			while (effectingCardsRefList.Num() > 0)
			{
				RemoveFromEffectingList(effectingCardsRefList[0]);
			}
			while (effectedByCardsRefList.Num() > 0)
			{
				effectedByCardsRefList[0]->RemoveFromEffectingList(this);
			}

			effectingCardsRefList.Empty();

			GetCardManager()->cardEffectManager->UnregisterActiveEffectCardInstance(this);

			PowerChangeCallback();

			return;
		}
		if (templateRef->IsType(CardType_Creature) || templateRef->HasEffect(CardEffect_UnsummonDummy))
		{
			/*cardFXManager.playCardDeployFX(this, UpdateEffectsApplied);*/
			UpdateEffectsApplied();
		}
		else if (templateRef->IsType(CardType_Weather))
		{
			if (templateRef->HasEffect(CardEffect_ClearSky))
			{
				cardsWeatherList = GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_WEATHERSLOT, PLAYER_INVALID);

				LogWarning("GFX - Applying Clear weather effect, numTargets: " + FString::FromInt(cardsWeatherList.Num()));

				while (cardsWeatherList.Num() > 0)
				{
					GetCardManager()->SendToGraveyard(cardsWeatherList[0]);
				}
			}
			else
			{
				cardsWeatherList = GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_WEATHERSLOT, PLAYER_INVALID);
				nCounter = 0;
				while (nCounter < cardsWeatherList.Num())
				{
					if (cardsWeatherList[nCounter]->templateRef == templateRef && cardsWeatherList[nCounter] != this)
					{
						GetCardManager()->SendToGraveyard(this);
						return;
					}
					++nCounter;
				}
			}
			/*cardFXManager.playCardDeployFX(this, UpdateEffectsApplied);*/
			UpdateEffectsApplied();
		}
		else
		{
			UpdateEffectsApplied();
		}
	}
}

int32 UCardInstance::GetTotalPower(bool bHasEffect)
{
	int32 nCounter = 0;
	int32 nHorn = 0;
	int32 nImproveNeighbours = 0;
	int32 nSameTypeMoral = 0;

	UCardInstance* effectedBy = nullptr;

	bool bIsWeatherCard = false;

	if (!templateRef->IsType(CardType_Hero))
	{
		while (nCounter < effectedByCardsRefList.Num())
		{
			effectedBy = effectedByCardsRefList[nCounter];

			if (effectedBy->templateRef->IsType(CardType_Weather))
			{
				bIsWeatherCard = true;
			}

			if (effectedBy->templateRef->HasEffect(CardEffect_Horn)
				|| effectedBy->templateRef->HasEffect(CardEffect_Siege_Horn)
				|| effectedBy->templateRef->HasEffect(CardEffect_Range_Horn)
				|| effectedBy->templateRef->HasEffect(CardEffect_Melee_Horn))
			{
				nHorn++;
			}

			if (effectedBy->templateRef->HasEffect(CardEffect_ImproveNeighbours))
			{
				nImproveNeighbours++;
			}

			if (effectedBy->templateRef->HasEffect(CardEffect_SameTypeMorale))
			{
				nSameTypeMoral++;
			}

			nCounter++;
		}
	}

	int32 nDefaultPower = !bHasEffect && bIsWeatherCard ? (FMath::Min(1, (GetCardManager()->GetCardTemplate(nTemplateId))->nPower)) : ((GetCardManager()->GetCardTemplate(nTemplateId))->nPower);
	int32 nActualPower = 0;

	nActualPower = nActualPower + nDefaultPower * nSameTypeMoral;
	nActualPower = nActualPower + nImproveNeighbours;
	if (nHorn > 0)
	{
		nActualPower = nActualPower + (nDefaultPower + nActualPower);
	}

	return nDefaultPower + nActualPower;
}

int32 UCardInstance::GetNotListPlayer()
{
	return nListsPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1);
}

int32 UCardInstance::GetNotOwningPlayer()
{
	return nOwningPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1);
}

int32 UCardInstance::PotentialWeatherHarm()
{
	TArray<UCardInstance*> creatureHandList;
	TArray<UCardInstance*> effectsList;
	UCardInstance* cardCreature = nullptr;

	int32 nWeatherHarm = 0;
	int32 nTotalPower = 0;
	int32 nCardLocation = 0;
	int32 nCounterEffect = 0;
	int32 nCounterCreature = 0;

	if (templateRef->IsType(CardType_Weather))
	{
		creatureHandList = GetCardManager()->GetAllCreaturesInHand(nListsPlayer);

		nWeatherHarm = 0;
		nTotalPower = 0;

		if (templateRef->HasEffect(CardEffect_Melee))
		{
			nCardLocation = CARD_LIST_LOC_MELEE;
		}
		else if (templateRef->HasEffect(CardEffect_Ranged))
		{
			nCardLocation = CARD_LIST_LOC_RANGED;
		}
		else if (templateRef->HasEffect(CardEffect_Siege))
		{
			nCardLocation = CARD_LIST_LOC_SIEGE;
		}

		effectsList = GetCardManager()->cardEffectManager->GetEffectsForList(nCardLocation, nListsPlayer);

		nCounterCreature = 0;
		while (nCounterCreature < creatureHandList.Num())
		{
			cardCreature = creatureHandList[nCounterCreature];

			if (cardCreature->templateRef->IsType(CardType_Creature)
				&& !cardCreature->templateRef->IsType(CardType_RangedMelee) && cardCreature->templateRef->IsType(nCardLocation))
			{
				nCounterEffect = 0;
				while (nCounterEffect < effectsList.Num())
				{
					cardCreature->effectedByCardsRefList.Add(effectsList[nCounterEffect]);

					nCounterEffect++;
				}

				nTotalPower = cardCreature->GetTotalPower();
				cardCreature->effectedByCardsRefList.Add(this);
				nWeatherHarm = nWeatherHarm + FMath::Max(0, FMath::FloorToInt(nTotalPower - cardCreature->GetTotalPower()));
				cardCreature->effectedByCardsRefList.Empty();
			}

			nCounterCreature++;
		}

		return nWeatherHarm;
	}
	else
	{
		return 0;
	}
}

UCardTransaction* UCardInstance::GetOptimalTransaction() const
{
	return _lastCalculatedPowerPotential;
}

bool UCardInstance::CanBeCastOn(UCardInstance* cardInstance)
{
	if (templateRef->IsType(CardType_Hero) || cardInstance->templateRef->IsType(CardType_Hero))
	{
		return false;
	}

	if (templateRef->HasEffect(CardEffect_UnsummonDummy)
		&& cardInstance->templateRef->IsType(CardType_Creature)
		&& cardInstance->nListsPlayer == nListsPlayer
		&& cardInstance->nInList != CARD_LIST_LOC_HAND
		&& cardInstance->nInList != CARD_LIST_LOC_GRAVEYARD
		&& cardInstance->nInList != CARD_LIST_LOC_LEADER)
	{
		return true;
	}

	return false;
}

bool UCardInstance::CanBePlacedInSlot(int32 nCardListLocation, int32 nPlayer)
{
	if (nCardListLocation == CARD_LIST_LOC_DECK || nCardListLocation == CARD_LIST_LOC_GRAVEYARD)
	{
		return false;
	}

	if (nPlayer == PLAYER_INVALID && nCardListLocation == CARD_LIST_LOC_WEATHERSLOT
		&& templateRef->IsType(CardType_Weather))
	{
		return true;
	}

	if (nPlayer == nListsPlayer && templateRef->IsType(CardType_Spy))
	{
		return false;
	}

	if (!templateRef->IsType(CardType_Spy) && nPlayer != nListsPlayer
		&& (templateRef->IsType(CardType_Creature) || templateRef->IsType(CardType_Row_Modifier)))
	{
		return false;
	}

	if (templateRef->IsType(CardType_Creature))
	{
		if (nCardListLocation == CARD_LIST_LOC_MELEE && templateRef->IsType(CardType_Melee))
		{
			return true;
		}

		if (nCardListLocation == CARD_LIST_LOC_RANGED && templateRef->IsType(CardType_Ranged))
		{
			return true;
		}

		if (nCardListLocation == CARD_LIST_LOC_SIEGE && templateRef->IsType(CardType_Siege))
		{
			return true;
		}
	}
	else if (templateRef->IsType(CardType_Row_Modifier))
	{
		if (nCardListLocation == CARD_LIST_LOC_MELEEMODIFIERS && templateRef->IsType(CardType_Melee)
			&& GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_MELEEMODIFIERS, nListsPlayer).Num() == 0)
		{
			return true;
		}

		if (nCardListLocation == CARD_LIST_LOC_RANGEDMODIFIERS && templateRef->IsType(CardType_Ranged)
			&& GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_RANGEDMODIFIERS, nListsPlayer).Num() == 0)
		{
			return true;
		}

		if (nCardListLocation == CARD_LIST_LOC_SIEGEMODIFIERS && templateRef->IsType(CardType_Siege)
			&& GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_SIEGEMODIFIERS, nListsPlayer).Num() == 0)
		{
			return true;
		}
	}

	return false;
}

void UCardInstance::AddToEffectingList(UCardInstance* cardInstance)
{
	effectingCardsRefList.Add(cardInstance);
	cardInstance->AddEffect(this);
}

void UCardInstance::AddEffect(UCardInstance* cardInstance)
{
	effectedByCardsRefList.Add(cardInstance);
	PowerChangeCallback();
}

void UCardInstance::RemoveFromEffectingList(UCardInstance* cardInstance)
{
	int32 nIndex = effectingCardsRefList.IndexOfByKey(cardInstance);
	if (nIndex != -1)
	{
		effectingCardsRefList.RemoveAt(nIndex);

		cardInstance->RemoveEffect(this);

		PowerChangeCallback();
	}
}

void UCardInstance::RemoveEffect(UCardInstance* cardInstance)
{
	int32 nIndex = effectedByCardsRefList.IndexOfByKey(cardInstance);
	if (nIndex != -1)
	{
		effectedByCardsRefList.RemoveAt(nIndex);

		PowerChangeCallback();
	}
}

void UCardInstance::SummonClones(UCardInstance* cardInstance)
{
	int32 nCounter = 0;

	while (nCounter < templateRef->summonFlags.Num())
	{
		GetCardManager()->SummonFromDeck(nListsPlayer, templateRef->summonFlags[nCounter]);
		GetCardManager()->SummonFromHand(nListsPlayer, templateRef->summonFlags[nCounter]);

		++nCounter;
	}
}

void UCardInstance::ResurrectCard(UCardInstance* cardInstance)
{
	if (cardInstance)
	{
		cardInstance->RecalculatePowerPotential(GetCardManager());
		GetCardManager()->AddCardInstanceToList(cardInstance, cardInstance->GetOptimalTransaction()->targetSlotID, cardInstance->GetOptimalTransaction()->targetPlayerID);
	}
}

void UCardInstance::HandleNurseChoice(int32 nIndex)
{
	UCardInstance* cardToResurrect = GetCardManager()->GetCardInstance(nIndex);

	//TODO play sound and fx resurrect
	//GetBoardRenderer()->PlaySound("gui_cards_resurrect");
	/*GetFXManager()->PlayRessurectEffectFX(cardToResurrect, onNurseEffectEnded);*/

	ResurrectCard(cardToResurrect);

	//TODO hide visual dialog if present (for human player)
}

void UCardInstance::PowerChangeCallback()
{
	UCardSlot* cardSlotOwner = GetBoardRenderer()->GetCardSlotFromCardInstance(this);

	if (cardSlotOwner)
		cardSlotOwner->UpdateCardPowerText();
}

void UCardInstance::UpdateEffectsApplied()
{
	int32 nCounter = 0;
	int32 nCardListLocation = 0;

	bool bTightBonds = false;
	bool bSummonClones = false;

	TArray<UCardInstance*> cardsList;
	TArray<UCardInstance*> resurrectionTargetsList;
	TArray<UCardInstance*> scorchTargetsList;
	TArray<UCardInstance*> scorchMeleeTargetsList;

	UCardsDeck* playerDeck = nullptr;
	UCardInstance* cardInstance = nullptr;

	FCardAndPointsCombo combo;

	LogWarning("GFX - updateEffectsApplied Called ----------");

	if (templateRef->IsType(CardType_Creature) && !templateRef->IsType(CardType_Hero))
	{
		cardsList.Empty();

		cardsList = GetCardManager()->cardEffectManager->GetEffectsForList(nInList, nListsPlayer);

		LogWarning("GFX - fetched: " + FString::FromInt(cardsList.Num()) + ", effects for list:" + FString::FromInt(nInList) + ", and Player:" + FString::FromInt(nListsPlayer));

		nCounter = 0;
		while (nCounter < cardsList.Num())
		{
			cardInstance = cardsList[nCounter];
			if (cardInstance != this)
			{
				cardInstance->AddToEffectingList(this);
			}
			++nCounter;
		}
	}

	if (templateRef->IsType(CardType_Weather))
	{
		if (!templateRef->HasEffect(CardEffect_ClearSky))
		{
			cardsList.Empty();

			if (templateRef->HasEffect(CardEffect_Melee))
			{
				GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, PLAYER_1, cardsList);
				GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, PLAYER_2, cardsList);

				GetCardManager()->cardEffectManager->RegisterActiveEffectCardInstance(this, CARD_LIST_LOC_MELEE, PLAYER_1);
				GetCardManager()->cardEffectManager->RegisterActiveEffectCardInstance(this, CARD_LIST_LOC_MELEE, PLAYER_2);

				LogWarning("GFX - Applying Melee Weather Effect");
			}
			if (templateRef->HasEffect(CardEffect_Ranged))
			{
				GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, PLAYER_1, cardsList);
				GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, PLAYER_2, cardsList);

				GetCardManager()->cardEffectManager->RegisterActiveEffectCardInstance(this, CARD_LIST_LOC_RANGED, PLAYER_1);
				GetCardManager()->cardEffectManager->RegisterActiveEffectCardInstance(this, CARD_LIST_LOC_RANGED, PLAYER_2);

				LogWarning("GFX - Applying Ranged Weather Effect");
			}
			if (templateRef->HasEffect(CardEffect_Siege))
			{
				GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, PLAYER_1, cardsList);
				GetCardManager()->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, PLAYER_2, cardsList);

				GetCardManager()->cardEffectManager->RegisterActiveEffectCardInstance(this, CARD_LIST_LOC_SIEGE, PLAYER_1);
				GetCardManager()->cardEffectManager->RegisterActiveEffectCardInstance(this, CARD_LIST_LOC_SIEGE, PLAYER_2);

				LogWarning("GFX - Applying SIEGE Weather Effect");
			}

			nCounter = 0;
			while (nCounter < cardsList.Num())
			{
				cardInstance = cardsList[nCounter];
				AddToEffectingList(cardInstance);
				++nCounter;
			}
		}
	}

	if (templateRef->HasEffect(CardEffect_Scorch))
	{
		scorchTargetsList = GetCardManager()->GetScorchTargets();

		LogWarning("GFX - Applying Scorch Effect, number of targets: " + FString::FromInt(scorchTargetsList.Num()));

		//TODO play scorch sound
		//GetBoardRenderer()->PlaySound("gui_cards_scorch");

		nCounter = 0;
		while (nCounter < scorchTargetsList.Num())
		{
			//TODO scorch effect
			/*GetCardFXManager().playScorchEffectFX(scorchTargetsList[nCounter], onScorchFXEnd);*/
			GetCardManager()->SendToGraveyard(scorchTargetsList[nCounter]);

			++nCounter;
		}
	}

	if (templateRef->HasEffect(CardEffect_MeleeScorch))
	{
		if (GetCardManager()->CalculatePlayerScore(CARD_LIST_LOC_MELEE, GetNotListPlayer()) >= 10)
		{
			scorchMeleeTargetsList = GetCardManager()->GetScorchTargets(CardType_Melee, GetNotListPlayer());

			LogWarning("GFX - Applying scorchMeleeList, number of targets: " + FString::FromInt(scorchMeleeTargetsList.Num()));

			//TODO play scorch sound
			//GetBoardRenderer()->PlaySound("gui_cards_scorch");

			nCounter = 0;
			while (nCounter < scorchMeleeTargetsList.Num())
			{
				//TODO scorch effect
				/*GetCardFXManager().playScorchEffectFX(scorchMeleeTargetsList[nCounter], onScorchFXEnd);*/
				GetCardManager()->SendToGraveyard(scorchMeleeTargetsList[nCounter]);

				++nCounter;
			}
		}
	}

	if (templateRef->HasEffect(CardEffect_Horn))
	{
		cardsList.Empty();

		LogWarning("GFX - Applying Horn Effect ----------");

		nCardListLocation = CARD_LIST_LOC_INVALID;

		if (nInList == CARD_LIST_LOC_MELEEMODIFIERS || nInList == CARD_LIST_LOC_MELEE)
		{
			nCardListLocation = CARD_LIST_LOC_MELEE;
		}
		else if (nInList == CARD_LIST_LOC_RANGEDMODIFIERS || nInList == CARD_LIST_LOC_RANGED)
		{
			nCardListLocation = CARD_LIST_LOC_RANGED;
		}
		else if (nInList == CARD_LIST_LOC_SIEGEMODIFIERS || nInList == CARD_LIST_LOC_SIEGE)
		{
			nCardListLocation = CARD_LIST_LOC_SIEGE;
		}

		if (nCardListLocation != PLAYER_INVALID)
		{
			cardsList = GetCardManager()->GetCardInstanceList(nCardListLocation, nListsPlayer);
			if (cardsList.Num() > 0)
			{
				nCounter = 0;

				while (nCounter < cardsList.Num())
				{
					cardInstance = cardsList[nCounter];
					if (!cardInstance->templateRef->IsType(CardType_Hero) && !(cardInstance == this))
					{
						AddToEffectingList(cardInstance);
					}
					++nCounter;
				}
			}

			//TODO play fx
// 			GetCardFXManager().playerCardEffectFX(this, nullptr);
// 			GetCardFXManager().playRowEffect(nCardListLocation, nListsPlayer);

			GetCardManager()->cardEffectManager->RegisterActiveEffectCardInstance(this, nCardListLocation, nListsPlayer);
		}
	}

	if (templateRef->HasEffect(CardEffect_Nurse))
	{
		GetCardManager()->GetRessurectionTargets(nListsPlayer, resurrectionTargetsList, true);

		LogWarning("GFX - Applying Nurse Effect");

		if (resurrectionTargetsList.Num() > 0)
		{
			if (GetGameFlowController()->playerControllers[nListsPlayer]->IsA(UAIPlayerController::StaticClass()))
			{
				combo = GetCardManager()->GetHigherOrLowerValueCardFromTargetGraveyard(nListsPlayer, true, true, false);
				cardInstance = combo.cardInstance;
				HandleNurseChoice(cardInstance->nInstanceId);
			}
			else
			{
				//TODO nurse for human player
				LogError("TODO interactive nurse for human player");
				/*GetGameFlowController()->mcChoiceDialog.showDialogCardInstances(resurrectionTargetsList, handleNurseChoice, noNurseChoice, "[[cards_choose_card_to_ressurect]]");*/

				//TEMP for now same as AI, while above not done
				combo = GetCardManager()->GetHigherOrLowerValueCardFromTargetGraveyard(nListsPlayer, true, true, false);
				cardInstance = combo.cardInstance;
				HandleNurseChoice(cardInstance->nInstanceId);
			}
		}
	}

	if (templateRef->HasEffect(CardEffect_ImproveNeighbours))
	{
		cardsList.Empty();

		cardsList = GetCardManager()->GetCardInstanceList(nInList, nListsPlayer);

		LogWarning("GFX - Applying Improve Neighbours effect");

		nCounter = 0;
		while (nCounter < cardsList.Num())
		{
			cardInstance = cardsList[nCounter];
			if (!cardInstance->templateRef->IsType(CardType_Hero) && !(cardInstance == this))
			{
				AddToEffectingList(cardInstance);
			}

			++nCounter;
		}
		//TODO card effect
		/*GetCardFXManager().playerCardEffectFX(this, nullptr);*/
		GetCardManager()->cardEffectManager->RegisterActiveEffectCardInstance(this, nInList, nListsPlayer);
	}

	if (templateRef->HasEffect(CardEffect_SameTypeMorale))
	{
		cardsList.Empty();

		GetCardManager()->GetAllCreaturesNonHero(nInList, nListsPlayer, cardsList);

		LogWarning("GFX - Applying Right Bonds effect");

		bTightBonds = false;

		nCounter = 0;
		while (nCounter < cardsList.Num())
		{
			cardInstance = cardsList[nCounter];
			if (!(cardInstance == this) && !(templateRef->summonFlags.IndexOfByKey(cardInstance->nTemplateId) == -1))
			{
				cardInstance->AddToEffectingList(this);
				AddToEffectingList(cardInstance);

				//TODO visual and sound
				//GetBoardRenderer()->PlaySound("gui_cards_morale_boost");
				//GetCardFXManager().playTightBondsFX(cardInstance, nullptr);

				bTightBonds = true;
			}
			++nCounter;
		}
		if (bTightBonds)
		{
			//TODO visual
			//GetCardFXManager().playTightBondsFX(cardInstance, nullptr);
		}
	}

	if (templateRef->HasEffect(CardEffect_SummonClones))
	{
		cardsList.Empty();

		cardsList = GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, nListsPlayer);

		playerDeck = GetCardManager()->playerDeckDefinitions[nListsPlayer];

		bSummonClones = false;

		nCounter = 0;
		while (nCounter < templateRef->summonFlags.Num() && !bSummonClones)
		{
			if (playerDeck->NumCopiesLeft(templateRef->summonFlags[nCounter]) > 0)
			{
				bSummonClones = true;
			}

			nCounter = 0;
			while (nCounter < cardsList.Num())
			{
				if (cardsList[nCounter]->nTemplateId == templateRef->summonFlags[nCounter])
				{
					bSummonClones = true;

					break;
				}

				++nCounter;
			}

			++nCounter;
		}

		FString sSummonClones = bSummonClones ? "True" : "False";
		LogWarning("GFX - Applying Summon Clones Effect, found summons: " + sSummonClones);

		if (bSummonClones)
		{
			//TODO visual
			/*GetCardFXManager().playerCardEffectFX(this, summonFXEnded);*/
			SummonClones(this);
		}
	}

	if (templateRef->HasEffect(CardEffect_Draw2))
	{
		LogWarning("GFX - applying draw 2 effect");

		GetCardManager()->DrawCards(nListsPlayer != PLAYER_1 ? PLAYER_1 : PLAYER_2, 2);
	}

	GetCardManager()->RecalculateScores();
}
