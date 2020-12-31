// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/CanvasPanel.h"
#include "CardsCardHolder.generated.h"

class UCardSlot;

/**
 * 
 */
UCLASS()
class CARDS_API UCardsCardHolder : public UCanvasPanel
{
	GENERATED_BODY()

public:
	UPROPERTY()
		int32 nCardHolder;
	UPROPERTY()
		int32 nPlayer;

	UPROPERTY()
		FString sTag;

	void CardAdded(UCardSlot* cardSlot);
	void RepositionAllCards();
	void RepositionAllCards_Graveyard();
	void RepositionAllCards_Standard(bool bFit);
	void OnPositionEnded(UCardSlot* cardSlot);
};
