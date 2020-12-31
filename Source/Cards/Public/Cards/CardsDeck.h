// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "CardsDeck.generated.h"

class UCardTemplate;

/**
 *
 */
UCLASS()
class CARDS_API UCardsDeck : public UObject
{
	GENERATED_BODY()

public:
	UCardsDeck(const FObjectInitializer& ObjectInitializer);

	bool TryDrawSpecificCard(int32 nCardIndex);
	
	int32 GetDeckFaction();
	int32 NumCopiesLeft(int32 nSummonFlag);
	int32 OriginalStength();
	int32 DrawCard();

	UCardTemplate* GetDeckKingTemplate();

	void GetCardsInDeck(int32 nCardType, int32 nCardEffect, TArray<int32>& cardsInDeckList);
	void ShuffleDeck(int32 nStrength);

	//in game deck
	TArray<int32> cardIndicesInDeck; 
	//deck builder (db) deck
	TArray<int32> cardIndices; 
	
	UPROPERTY()
		FString	sDeckName;

	UPROPERTY()
		int32 nSelectedKingIndex;
	UPROPERTY()
		int32 nSpecialCard;

	UPROPERTY()
		bool bIsUnlocked = false;

};
