// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsRoundResult.h"

#include "ldf.h"

UCardsRoundResult::UCardsRoundResult(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UCardsRoundResult::SetResults(int32 nScoreP1, int32 nScoreP2, int32 nWinner)
{
	if(Played())
	{
		LogError("GFX - Tried to set round results on a round that already had results!");
	}

	roundScores.Empty();

	roundScores.Add(nScoreP1);
	roundScores.Add(nScoreP2);
	roundWinner = nWinner;
}

void UCardsRoundResult::Reset()
{
	roundScores.Empty();
}

bool UCardsRoundResult::Played()
{
	return roundScores.Num() > 0;
}

int32 UCardsRoundResult::GetRoundWinningPlayer()
{
	if (roundWinner != -1)
	{
		return roundWinner;
	}
	if (Played())
	{
		if (roundScores[PLAYER_1] == roundScores[PLAYER_2])
		{
			return PLAYER_INVALID;
		}
		if (roundScores[PLAYER_1] > roundScores[PLAYER_2])
		{
			return PLAYER_1;
		}
		return PLAYER_2;
	}
	return PLAYER_INVALID;
}

FString UCardsRoundResult::ToString()
{
	if (Played())
	{
		return "[ROUND RESULT] p1Score: " + FString::FromInt(roundScores[0]) + ", p2Score: " + FString::FromInt(roundScores[1]) + ", roundWinner: " + FString::FromInt(roundWinner);
	}
	return "[ROUND RESULT] empty!";
}


