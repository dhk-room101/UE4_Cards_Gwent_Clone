// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CardsSaveGame.generated.h"

/**
 *
 */
UCLASS()
class CARDS_API UCardsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	//TODO games played, won, lost, etc.
	UPROPERTY()
		TMap<int32, int32> CardsHolderAll;//index, qty
	UPROPERTY()
		TMap<int32, int32> CardsHolderDeck;//index, qty
	UPROPERTY()
		int32 nFactionLeaderP1;
	UPROPERTY()
		int32 nWins;
	UPROPERTY()
		int32 nLoses;
	UPROPERTY()
		int32 nDraws;
};
