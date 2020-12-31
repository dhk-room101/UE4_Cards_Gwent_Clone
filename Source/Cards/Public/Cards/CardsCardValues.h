// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "CardsCardValues.generated.h"

/**
 *
 */
UCLASS()
class CARDS_API UCardsCardValues : public UObject
{
	GENERATED_BODY()

public:
	UCardsCardValues(const FObjectInitializer& ObjectInitializer);
	TMap<int32, int32> GetEffectValueDictionary();

	UPROPERTY()
		float weatherCardValue = 2;
	UPROPERTY()
		float hornCardValue = 5;
	UPROPERTY()
		float drawCardValue = 6;
	UPROPERTY()
		float scorchCardValue = 6;
	UPROPERTY()
		float summonClonesCardValue = 3;
	UPROPERTY()
		float unsummonCardValue = 4;
	UPROPERTY()
		float improveNeighboursCardValue = 4;
	UPROPERTY()
		float nurseCardValue = 4;


private:
	TMap<int32, int32> _bufferedDictionary;
};
