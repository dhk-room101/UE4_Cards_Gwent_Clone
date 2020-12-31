// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsCardValues.h"

#include "ldf.h"

UCardsCardValues::UCardsCardValues(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

TMap<int32, int32> UCardsCardValues::GetEffectValueDictionary()
{
	if (_bufferedDictionary.Num() == 0)
	{
		_bufferedDictionary.Add(CardEffect_Horn) = hornCardValue;
		_bufferedDictionary.Add(CardEffect_Draw) = drawCardValue;
		_bufferedDictionary.Add(CardEffect_Draw2) = drawCardValue * 2;
		_bufferedDictionary.Add(CardEffect_Scorch) = scorchCardValue;
		_bufferedDictionary.Add(CardEffect_SummonClones) = summonClonesCardValue;
		_bufferedDictionary.Add(CardEffect_UnsummonDummy) = unsummonCardValue;
		_bufferedDictionary.Add(CardEffect_ImproveNeighbours) = improveNeighboursCardValue;
		_bufferedDictionary.Add(CardEffect_Nurse) = nurseCardValue;
	}

	return _bufferedDictionary;
}


