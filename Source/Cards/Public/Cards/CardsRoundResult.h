// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "CardsRoundResult.generated.h"

/**
 *
 */
UCLASS()
class CARDS_API UCardsRoundResult : public UObject
{
	GENERATED_BODY()

public:
	UCardsRoundResult(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE TArray<int32> GetRoundScores() const { return roundScores; }

	void SetResults(int32 nScoreP1, int32 nScoreP2, int32 nWinner);
	void Reset();

	bool Played();

	int32 GetRoundWinningPlayer();

	FString ToString();

private:
	TArray<int32> roundScores;

	UPROPERTY()
		int32 roundWinner;
};
