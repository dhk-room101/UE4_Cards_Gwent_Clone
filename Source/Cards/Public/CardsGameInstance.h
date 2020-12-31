// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CardTemplate.h"

#include "Engine/GameInstance.h"
#include "CardsGameInstance.generated.h"

class UCardsGameFlowController;
class UCardManager;
class UCardsDeck;
class UCardsSaveGame;

/**
 *
 */
UCLASS()
class CARDS_API UCardsGameInstance : public UGameInstance
{
	GENERATED_BODY()

		UCardsGameInstance(const FObjectInitializer& ObjectInitializer);

public:

	inline static bool PowerSorter(const UCardTemplate& card1, const UCardTemplate& card2)
	{
		return card1.nPower < card2.nPower;
	}

	void StartGame();

	UPROPERTY()
		UCardsGameFlowController* gameFlowController;
	UPROPERTY()
		UCardManager* cardManager;

	int32 nWins;
	int32 nLoses;
	int32 nDraws;

private:
	void SetCardTemplates();
	void SetLeaderCardTemplates();

	void TempFunction(int32 nTemp);

	void AddIndicesToDeck(int32 nIndex);

	void InitializeBoard();
	void LoadCardsFromSave(UCardsSaveGame* LoadGameInstance);

	int32 nFactionLeaderP1;
	int32 nFactionLeaderP2;
	int32 maxDeckCardPower;

	TArray<UCardsDeck*> decks;
};
