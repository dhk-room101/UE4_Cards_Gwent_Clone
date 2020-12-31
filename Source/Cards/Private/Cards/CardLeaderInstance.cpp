// Fill out your copyright notice in the Description page of Project Settings.

#include "CardLeaderInstance.h"

#include "ldf.h"
#include "STypes.h"

#include "CardManager.h"
#include "CardTemplate.h"

#include "CardsCardValues.h"
#include "CardsDeck.h"
#include "CardsGameMain.h"

#include "Kismet/KismetMathLibrary.h"

UCardLeaderInstance::UCardLeaderInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UCardLeaderInstance::FinalizeSetup()
{
	Super::FinalizeSetup();

	if (templateRef == nullptr || templateRef->GetFirstEffect() == CardEffect_None)
	{
		LogError("GFX [ERROR] tried to finalize card leader with invalid template info - " + FString::FromInt(nTemplateId));
	}
	nLeaderEffect = templateRef->GetFirstEffect();
	if (nLeaderEffect == CardEffect_Counter_King)
	{
		bCanBeUsed = false;
		GetBoardRenderer()->DisableLeaderUse(nOwningPlayer);
	}
}

void UCardLeaderInstance::RecalculatePowerPotential(UCardManager * cardManager)
{
	Super::RecalculatePowerPotential(cardManager);

	UCardInstance* currentCard = nullptr;
	FCardAndPointsCombo cardAndPointsCombo;

	int32 nPoints = 0;
	int32 nPowerDifference = 0;
	int32 nPowerTotal = 0;
	int32 nCounter = 0;

	_lastCalculatedPowerPotential->powerChangeResult = 0;
	_lastCalculatedPowerPotential->strategicValue = -1;
	_lastCalculatedPowerPotential->sourceCardInstanceRef = Cast<UCardInstance>(this);

	UCardManager* instanceCardManager = GetCardManager();

	TArray<UCardInstance*> creaturesList;

	int32 _player = nOwningPlayer == PLAYER_1 ? (PLAYER_2) : (PLAYER_1);

	TArray<UCardInstance*>& cardsHandList = instanceCardManager->GetCardInstanceList(CARD_LIST_LOC_HAND, nOwningPlayer);

	switch (templateRef->GetFirstEffect())
	{
	case CardEffect_11th_card:
	case CardEffect_Counter_King:
	{
		break;
	}
	case CardEffect_Pick_Fog:
	case CardEffect_Pick_Frost:
	case CardEffect_Pick_Weather:
	case CardEffect_Pick_Rain:
	{
		_lastCalculatedPowerPotential->strategicValue = instanceCardManager->cardValues->weatherCardValue;
		
		break;
	}
	case CardEffect_View_3_Enemy:
	{
		_lastCalculatedPowerPotential->strategicValue = 0;

		break;
	}
	case CardEffect_Siege_Horn:
	case CardEffect_Range_Horn:
	case CardEffect_Melee_Horn:
	{
		if (templateRef->GetFirstEffect() == CardEffect_Siege_Horn)
		{
			instanceCardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nOwningPlayer, creaturesList);
		}
		else if (templateRef->GetFirstEffect() == CardEffect_Range_Horn)
		{
			instanceCardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nOwningPlayer, creaturesList);
		}
		else if (templateRef->GetFirstEffect() == CardEffect_Melee_Horn)
		{
			instanceCardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nOwningPlayer, creaturesList);
		}
		
		nPowerDifference = 0;
		nCounter = 0;
		
		while (nCounter < creaturesList.Num())
		{
			currentCard = creaturesList[nCounter];
			nPowerTotal = currentCard->GetTotalPower();
			
			currentCard->effectedByCardsRefList.Add(this);
			
			nPowerDifference = currentCard->GetTotalPower() - nPowerTotal;
			
			currentCard->effectedByCardsRefList.Pop();
			
			nCounter++;
		}
		
		_lastCalculatedPowerPotential->powerChangeResult = nPowerDifference;
		_lastCalculatedPowerPotential->strategicValue = instanceCardManager->cardValues->hornCardValue;
		
		break;
	}
	case CardEffect_Range_Scorch:
	{
		creaturesList = instanceCardManager->GetScorchTargets(CardType_Ranged, _player);
		nCounter = 0;
		
		while (nCounter < creaturesList.Num())
		{
			currentCard = creaturesList[nCounter];

			if (currentCard->nListsPlayer == nOwningPlayer)
			{
				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult - currentCard->GetTotalPower();
			}
			else
			{
				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + currentCard->GetTotalPower();
			}

			++nCounter;
		}

		_lastCalculatedPowerPotential->strategicValue = instanceCardManager->cardValues->scorchCardValue;

		break;
	}
	case CardEffect_MeleeScorch:
	{
		creaturesList = instanceCardManager->GetScorchTargets(CardType_Melee, _player);
		nCounter = 0;
		
		while (nCounter < creaturesList.Num())
		{
			currentCard = creaturesList[nCounter];

			if (currentCard->nListsPlayer == nOwningPlayer)
			{
				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult - currentCard->GetTotalPower();
			}
			else
			{
				_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + currentCard->GetTotalPower();
			}

			++nCounter;
		}

		_lastCalculatedPowerPotential->strategicValue = instanceCardManager->cardValues->scorchCardValue;

		break;
	}
	case CardEffect_Clear_Weather:
	{
		creaturesList.Empty();
		
		cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, nOwningPlayer, creaturesList);
		cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, nOwningPlayer, creaturesList);
		cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, nOwningPlayer, creaturesList);
		
		nCounter = 0;
		
		while (nCounter < creaturesList.Num())
		{

			_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult + (currentCard->GetTotalPower(true) - currentCard->GetTotalPower());
		
			++nCounter;
		}

		creaturesList.Empty();
		
		cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_MELEE, _player, creaturesList);
		cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_RANGED, _player, creaturesList);
		cardManager->GetAllCreaturesNonHero(CARD_LIST_LOC_SIEGE, _player, creaturesList);
		
		nCounter = 0;
		
		while (nCounter < creaturesList.Num())
		{
			_lastCalculatedPowerPotential->powerChangeResult = _lastCalculatedPowerPotential->powerChangeResult - (currentCard->GetTotalPower(true) - currentCard->GetTotalPower());
		
			++nCounter;
		}
		
		_lastCalculatedPowerPotential->strategicValue = instanceCardManager->cardValues->weatherCardValue;
		
		break;
	}
	case CardEffect_Resurect_Enemy:
	{
		cardAndPointsCombo = instanceCardManager->GetHigherOrLowerValueCardFromTargetGraveyard(_player, true, true, false, true);

		if (cardAndPointsCombo.cardInstance != nullptr)
		{
			nPoints = cardAndPointsCombo.comboPoints;

			if (cardsHandList.Num() < 8 || UKismetMathLibrary::RandomFloatInRange(0, 1) <= 1 / cardsHandList.Num() * 0.5)
			{
				_lastCalculatedPowerPotential->strategicValue = FMath::Max(0, 10 - nPoints);
				_lastCalculatedPowerPotential->powerChangeResult = cardAndPointsCombo.cardInstance->GetTotalPower();
			}
			else
			{
				_lastCalculatedPowerPotential->strategicValue = 1000;
			}
		}

		break;
	}
	case CardEffect_Resurect:
	{
		cardAndPointsCombo = instanceCardManager->GetHigherOrLowerValueCardFromTargetGraveyard(nOwningPlayer, true, true, false);
		
		if (cardAndPointsCombo.cardInstance != nullptr)
		{
			nPoints = cardAndPointsCombo.comboPoints;
			
			if (cardsHandList.Num() < 8 || UKismetMathLibrary::RandomFloatInRange(0, 1) <= 1 / cardsHandList.Num() * 0.5)
			{
				_lastCalculatedPowerPotential->strategicValue = FMath::Max(0, 10 - nPoints);
				_lastCalculatedPowerPotential->powerChangeResult = cardAndPointsCombo.cardInstance->GetTotalPower();
			}
			else
			{
				_lastCalculatedPowerPotential->strategicValue = 1000;
			}
		}

		break;
	}
	case CardEffect_Bin2_Pick1:
	{
		break;
	}
	default:
	{
		break;
	}
	}
}

void UCardLeaderInstance::ApplyLeaderAbility(bool bIsAI)
{
	//TODO visual disable leader usability

	if (!bCanBeUsed)
	{
		LogError("GFX [ERROR] - Tried to apply a card ability when its disabled!");

		return;
	}

	FCardAndPointsCombo combo;
	
	UCardsDeck* playerDeck = GetCardManager()->playerDeckDefinitions[nOwningPlayer];
	
	TArray<int32> cardsInDeckList;
	
	bCanBeUsed = false;
	GetBoardRenderer()->DisableLeaderUse(nOwningPlayer);

	//TODO play sound
	
	switch (templateRef->GetFirstEffect())
	{
	case CardEffect_Clear_Weather:
	{
		ClearWeather();
		
		break;
	}
	case CardEffect_Pick_Fog:
	{
		playerDeck->GetCardsInDeck(CardType_Weather, CardEffect_Ranged, cardsInDeckList);
		if (cardsInDeckList.Num() > 0)
		{
			GetCardManager()->TryDrawAndPlaySpecificCard_Weather(nOwningPlayer, cardsInDeckList[0]);
		}
		else
		{
			LogError("GFX [ERROR] - tried to pick fog but did not have any");
		}
		
		break;
	}
	case CardEffect_Siege_Horn:
	{
		ApplyHorn(CARD_LIST_LOC_SIEGEMODIFIERS, nOwningPlayer);
		break;
	}
	case CardEffect_Range_Scorch:
	{
		Scorch(CardType_Ranged);
		break;
	}
	case CardEffect_Pick_Frost:
	{
		playerDeck->GetCardsInDeck(CardType_Weather, CardEffect_Melee, cardsInDeckList);
		if (cardsInDeckList.Num() > 0)
		{
			GetCardManager()->TryDrawAndPlaySpecificCard_Weather(nOwningPlayer, cardsInDeckList[0]);
		}
		else
		{
			LogError("GFX [ERROR] - tried to pick frost but did not have any");
		}
		break;
	}
	case CardEffect_Range_Horn:
	{
		ApplyHorn(CARD_LIST_LOC_RANGEDMODIFIERS, nOwningPlayer);
		break;
	}
	case CardEffect_11th_card:
	{
		LogError("GFX [ERROR] - tried to apply 11th card ability which should not occur through here");
	}
	case CardEffect_MeleeScorch:
	{
		Scorch(CardType_Melee);
		break;
	}
	case CardEffect_Pick_Rain:
	{
		playerDeck->GetCardsInDeck(CardType_Weather, CardEffect_Siege, cardsInDeckList);
		if (cardsInDeckList.Num() > 0)
		{
			GetCardManager()->TryDrawAndPlaySpecificCard_Weather(nOwningPlayer, cardsInDeckList[0]);
		}
		else
		{
			LogError("GFX [ERROR] - tried to pick Rain but did not have any");
		}
		break;
	}
	case CardEffect_View_3_Enemy:
	{
		if (!bIsAI)
		{
			ShowEnemyHand(3);
		}
		break;
	}
	case CardEffect_Resurect_Enemy:
	{
		if (bIsAI)
		{
			combo = GetCardManager()->GetHigherOrLowerValueCardFromTargetGraveyard(GetNotOwningPlayer(), true, true, false, true);
			if (combo.cardInstance)
			{
				HandleResurrectChoice(combo.cardInstance->nInstanceId);
			}
			else
			{
				LogError("GFX [ERROR] - AI tried to resurrect enemy card when there wasn't a valid target!");
			}
		}
		else
		{
			ResurrectGraveyard(GetNotOwningPlayer());
		}
		break;
	}
	case CardEffect_Counter_King:
	{
		LogError("GFX [ERROR] - tried to apply counter king ability which should not occur through here");
	}
	case CardEffect_Bin2_Pick1:
	{
		if (bIsAI)
		{
			LogWarning("GFX [WARNING] - AI tried to bin2, pick 1 but it was never properly implemented");
		}
		else
		{
			BinPick(2, 1);
		}
		break;
	}
	case CardEffect_Pick_Weather:
	{
		PickWeather(bIsAI);
		break;
	}
	case CardEffect_Resurect:
	{
		if (bIsAI)
		{
			combo = GetCardManager()->GetHigherOrLowerValueCardFromTargetGraveyard(nOwningPlayer, true, true, false);
			if (combo.cardInstance)
			{
				HandleResurrectChoice(combo.cardInstance->nInstanceId);
			}
			else
			{
				LogError("GFX [ERROR] - AI tried to resurrect enemy card when there wasn't a valid target!");
			}
		}
		else
		{
			ResurrectGraveyard(nOwningPlayer);
		}
		break;
	}
	case CardEffect_Melee_Horn:
	{
		ApplyHorn(CARD_LIST_LOC_MELEEMODIFIERS, nOwningPlayer);
		break;
	}
	default:
	{
		break;
	}
	}


	LogWarning("Leader was used!");


	GetBoardRenderer()->ClearCardTransactionZoom();
}

void UCardLeaderInstance::ClearWeather()
{
	//TODO visual
	ApplyClearWeather();
}

void UCardLeaderInstance::ApplyClearWeather()
{
	TArray<UCardInstance*>& cardsWeatherList = GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_WEATHERSLOT, PLAYER_INVALID);
	
	if (cardsWeatherList.Num() > 0)
	{
		GetCardManager()->SendToGraveyard(cardsWeatherList[0]);
	}
}

void UCardLeaderInstance::ApplyHorn(int32 nCardListLocation, int32 nPlayer)
{
	UCardInstance* cardInstance = GetCardManager()->SpawnCardInstance(1, nPlayer, CARD_LIST_LOC_LEADER);
	GetCardManager()->AddCardInstanceToList(cardInstance, nCardListLocation, nPlayer);
}

void UCardLeaderInstance::Scorch(int32 nCardType)
{

	LogWarning("Leader Scorch Ability");


	
	int32 nCounter = 0;
	
	TArray<UCardInstance*> scorchList = GetCardManager()->GetScorchTargets(nCardType, GetNotOwningPlayer());
	
	//TODO sound
	while (nCounter < scorchList.Num())
	{
		//TODO visual and Timer End
		GetCardManager()->SendToGraveyard(scorchList[nCounter]);

		nCounter++;
	}
}

void UCardLeaderInstance::ShowEnemyHand(int32 nCardCount)
{
	TArray<UCardInstance*> tempCardsHandList;
	TArray<UCardInstance*> cardsToShowList;
	TArray<UCardInstance*>& cardsHandList = GetCardManager()->GetCardInstanceList(CARD_LIST_LOC_HAND, GetNotOwningPlayer());

	int32 nCounter = 0;
	int32 nRandomCardIndex = 0;
	int32 nCards = nCardCount;

	while (nCounter < cardsHandList.Num())
	{
		tempCardsHandList.Add(cardsHandList[nCounter]);
		
		nCounter++;
	}

	while (nCards > 0 && tempCardsHandList.Num() > 0)
	{
		nRandomCardIndex = FMath::Min(FPlatformMath::FloorToInt(UKismetMathLibrary::RandomFloatInRange(0, 1) * tempCardsHandList.Num()), (tempCardsHandList.Num() - 1));
		cardsToShowList.Add(tempCardsHandList[nRandomCardIndex]);
		tempCardsHandList.RemoveAt(nRandomCardIndex);
		
		nCards--;
	}

	if (cardsToShowList.Num() > 0)
	{
		//TODO show enemy cards

		LogWarning("show enemy cards");

	}
	else
	{
		LogError("GFX [ERROR] - Tried to ShowEnemyHand with no cards chosen?! - " + FString::FromInt(cardsHandList.Num()));
	}
}

void UCardLeaderInstance::HandleResurrectChoice(int32 nCardInstanceID)
{
	//TODO visual
	if (nCardInstanceID == -1)
	{
		LogError("GFX [ERROR] - tried to resurrect card with no valid id");
	}
	GetCardManager()->AddCardInstanceIDToList(nCardInstanceID, CARD_LIST_LOC_HAND, nOwningPlayer);
}

void UCardLeaderInstance::ResurrectGraveyard(int32 nPlayer)
{
	TArray<UCardInstance*> ressurectionTargetsList;
	
	GetCardManager()->GetRessurectionTargets(nPlayer, ressurectionTargetsList, true);
	
	if (ressurectionTargetsList.Num() > 0)
	{
		if (ressurectionTargetsList.Num() == 1)
		{
			GetCardManager()->AddCardInstanceToList(ressurectionTargetsList[0], CARD_LIST_LOC_HAND, nOwningPlayer);
		}
		else
		{
			//TODO visual
			LogWarning("[[cards_choose_card_to_ressurect]]");
		}
	}
	else
	{
		LogError("GFX [ERROR] - tried to resurrect from player: " + FString::FromInt(nPlayer) + "'s graveyard but found no cards");
	}
}

void UCardLeaderInstance::BinPick(int32 nBin, int32 nPick)
{
	//TODO visual

	LogError("Implement BinPick!");


}

void UCardLeaderInstance::PickWeather(bool bIsAI)
{
	UCardsDeck* cardsDeck = GetCardManager()->playerDeckDefinitions[nOwningPlayer];
	
	TArray<int32> cardsinDeckList;
	cardsDeck->GetCardsInDeck(CardType_Weather, CardEffect_None, cardsinDeckList);
	
	if (cardsinDeckList.Num() == 1 || bIsAI && cardsinDeckList.Num() > 0)
	{
		GetCardManager()->TryDrawAndPlaySpecificCard_Weather(nOwningPlayer, cardsinDeckList[0]);
	}
	else if (cardsinDeckList.Num() > 0)
	{
		//TODO visual
		LogWarning("[[cards_pick_card_to_draw]]");
	}
	else
	{
		LogError("GFX [ERROR] - tried to pick weather card when there was none");
	}
}

