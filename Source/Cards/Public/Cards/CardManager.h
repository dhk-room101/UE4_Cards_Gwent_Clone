// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CardTemplate.h"
#include "CardInstance.h"

#include "CardsDeck.h"
#include "CardsRoundResult.h"

#include "ldf.h"
#include "STypes.h"

#include "UObject/NoExportTypes.h"
#include "CardManager.generated.h"

class UCardEffectManager;
class UCardsCardValues;
class UCardLeaderInstance;

/**
*
*/
UCLASS()
class CARDS_API UCardManager : public UObject
{
	GENERATED_BODY()

public:
	UCardManager(const FObjectInitializer& ObjectInitializer);

	inline static bool CardSorter(const UCardInstance& card1, const UCardInstance& card2)
	{
		UCardTemplate* templateCard1 = card1.templateRef;
		UCardTemplate* templateCard2 = card2.templateRef;
		
		int32 creatureTypeCard1 = templateCard1->GetCreatureType();
		int32 creatureTypeCard2 = templateCard2->GetCreatureType();
		
		if (creatureTypeCard1 == CardType_None && creatureTypeCard2 == CardType_None)
		{
			return card1.nTemplateId < card2.nTemplateId;
		}
		
		if (templateCard1->nPower != templateCard2->nPower)
		{
			return templateCard1->nPower < templateCard2->nPower;
		}

		return card1.nTemplateId < card2.nTemplateId;
	}

	void SpawnLeaders();
	void AddCardInstanceToList(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer);
	void AddCardInstanceIDToList(int32 nCardInstanceID, int32 nCardListLocation, int32 nPlayer);
	void RemoveCardInstanceFromList(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer);
	void RemoveCardInstanceFromItsList(UCardInstance* cardInstance);
	void RecalculateScores();
	void UpdatePlayerLives();
	void ClearBoard(bool bExclude);
	void SendListToGraveyard(int32 nCardListLocation, int32 nPlayer, UCardInstance* cardInstance);
	void SendToGraveyard(UCardInstance* cardInstance);
	void CalculateCardPowerPotentials();
	void ShuffleAndDrawCards();
	void SummonFromHand(int32 nPlayer, int32 nCardIndex);

	//debug
	void TraceRoundResults();

	UCardInstance* SpawnCardInstance(int32 cardIndex, int32 nPlayer, int32 nCardListLocation = CARD_LIST_LOC_INVALID);
	UCardInstance* ChooseCreatureToExclude(int32 nPlayer);
	UCardInstance* GetFirstCardInHandWithEffect(int32 nCardEffect, int32 nPlayer);
	UCardInstance* GetHigherOrLowerValueTargetCardOnBoard(UCardInstance* cardInstance, int32 nPlayer, bool param3 = true, bool param4 = false, bool param5 = false);
	UCardInstance* GetCardInstance(int32 nCardIndex);
	UCardTemplate* GetCardTemplate(int32 nCardIndex);
	UCardLeaderInstance* GetCardLeader(int32 nPlayer);

	TArray<UCardInstance*>& GetCardInstanceList(int32 nCardListLocation, int32 nPlayer);
	TArray<UCardInstance*> GetAllCreatures(int32 nPlayer);
	TArray<UCardInstance*> GetAllCreaturesInHand(int32 nPlayer);
	TArray<UCardInstance*> GetCardsInSlotIdWithEffect(int32 nCardEffect, int32 nPlayer, int32 nCardListLocation = CARD_LIST_LOC_INVALID);
	TArray<UCardInstance*> GetCardsInHandWithEffect(int32 nCardEffect, int32 nPlayer);
	TArray<UCardInstance*> GetScorchTargets(int32 nCardType = CardType_SiegeRangedMelee, int32 nPlayer = PLAYER_BOTH);
	void GetAllCreaturesNonHero(int32 nCardListLocation, int32 nPlayer, TArray<UCardInstance*>& cardList);
	void GetRessurectionTargets(int32 nPlayer, TArray<UCardInstance*>& targetsList, bool bUpdate);
	void Reset();

	bool IsBetterMatchForGrave(UCardInstance* cardInstance, UCardInstance* cardCompareTo, int32 nPlayer, bool param4, bool param5, bool param6);
	bool IsBetterMatch(UCardInstance* param1, UCardInstance* param2, int32 param3, bool param4, bool param5, bool param6);
	bool CheckIfHigherOrLower(UCardInstance* cardInstance, UCardInstance* cardCompareTo, bool bHigher);
	bool TryDrawAndPlaySpecificCard_Weather(int32 nPlayer, int32 nCardIndex);
	bool TryDrawSpecificCard(int32 nPlayer, int32 nCardIndex);
	bool DrawCards(int32 nPlayer, int32 nCardCounter);
	bool DrawCard(int32 nPlayer);
	bool SummonFromDeck(int32 nPlayer, int32 nCardIndex);

	FCardAndPointsCombo GetHigherOrLowerValueCardFromTargetGraveyard(int32 nPlayer, bool param2 = true, bool param3 = false, bool param4 = false, bool bEnemy = false);

	FString ListIDToString(int32 nList);

	int32 GetWinningPlayer();
	int32 CalculatePlayerScore(int32 nCardListLocation, int32 nPlayer);

	UPROPERTY()
		UCardEffectManager* cardEffectManager = nullptr;
	UPROPERTY()
		UCardsCardValues* cardValues = nullptr;

	TMap<int32, UCardTemplate*> _cardTemplates;
	TMap<int32, UCardInstance*> _cardInstances;

	TArray<UCardsDeck*> playerDeckDefinitions;
	TArray<UCardsRoundResult*> roundResults;

	TArray<int32> currentPlayerScores;

	UPROPERTY()
		bool cardTemplatesReceived;

	//pseudo-static
	UPROPERTY()
		int32 nLastInstance = 0;

private:
	void InitializeLists();

	TMap<int32, TArray<UCardInstance*>> cardListHand;
	TMap<int32, TArray<UCardInstance*>> cardListGraveyard;
	TMap<int32, TArray<UCardInstance*>> cardListSiege;
	TMap<int32, TArray<UCardInstance*>> cardListRanged;
	TMap<int32, TArray<UCardInstance*>> cardListMelee;
	TMap<int32, TArray<UCardInstance*>> cardListSiegeModifier;
	TMap<int32, TArray<UCardInstance*>> cardListRangedModifier;
	TMap<int32, TArray<UCardInstance*>> cardListMeleeModifier;
	TMap<int32, TArray<UCardInstance*>> cardListLeader;
	TArray<UCardInstance*> cardListWeather;

	//TODO deprecate? update?
	UPROPERTY()
		bool _TEMP_tutorialsOn = false;
};
