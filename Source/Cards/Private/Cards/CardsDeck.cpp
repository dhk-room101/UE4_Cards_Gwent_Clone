// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsDeck.h"

#include "CardTemplate.h"
#include "CardManager.h"

#include "CardsGameMain.h"

#include "ldf.h"

#include "Kismet/KismetMathLibrary.h"

UCardsDeck::UCardsDeck(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

bool UCardsDeck::TryDrawSpecificCard(int32 nCardIndex)
{
	int32 nCounter = 0;
	while (nCounter < cardIndicesInDeck.Num())
	{
		if (cardIndicesInDeck[nCounter] == nCardIndex)
		{
			cardIndicesInDeck.RemoveAt(nCounter);

			//visual
			GetBoardRenderer()->UpdateDeckCounter();
			
			return true;
		}
		
		nCounter++;
	}

	return false;
}

int32 UCardsDeck::GetDeckFaction()
{
	UCardTemplate* cardTemplate = GetDeckKingTemplate();
	if (cardTemplate)
	{
		return cardTemplate->nFactionIdx;
	}
	return FactionId_Error;
}

int32 UCardsDeck::NumCopiesLeft(int32 nSummonFlag)
{
	int32 nCounter = 0;
	int32 nLeft = 0;
	
	while (nCounter < cardIndicesInDeck.Num())
	{
		if (nSummonFlag == cardIndicesInDeck[nCounter])
		{
			nLeft++;
		}
		
		nCounter++;
	}
	
	return nLeft;
}

int32 UCardsDeck::OriginalStength()
{
	UCardTemplate* cardTemplate = nullptr;
	
	int32 nCounter = 0;
	int32 nStrength = 0;
	
	while (nCounter < cardIndices.Num())
	{
		cardTemplate = GetCardManager()->GetCardTemplate(cardIndices[nCounter]);
		
		if (cardTemplate->IsType(CardType_Creature))
		{
			nStrength = nStrength + cardTemplate->nPower;
		}
		
		switch (cardTemplate->GetFirstEffect())
		{
		case CardEffect_Melee:
		case CardEffect_Ranged:
		case CardEffect_Siege:
		case CardEffect_ClearSky:
		{
			nStrength = nStrength + 2;
			break;
		}
		case CardEffect_UnsummonDummy:
		{
			nStrength = nStrength + 4;
			break;
		}
		case CardEffect_Horn:
		{
			nStrength = nStrength + 5;
			break;
		}
		case CardEffect_Scorch:
		case CardEffect_MeleeScorch:
		{
			nStrength = nStrength + 6;
			break;
		}
		case CardEffect_SummonClones:
		{
			nStrength = nStrength + 3;
			break;
		}
		case CardEffect_ImproveNeighbours:
		{
			nStrength = nStrength + 4;
			break;
		}
		case CardEffect_Nurse:
		{
			nStrength = nStrength + 4;
			break;
		}
		case CardEffect_Draw2:
		{
			nStrength = nStrength + 6;
			break;
		}
		case CardEffect_SameTypeMorale:
		{
			nStrength = nStrength + 4;
			break;
		}
		default:
		{
			break;
		}
		}

		nCounter++;
	}

	LogWarning("GFX -#AI#----- > Original Strength: " + FString::FromInt(nStrength));

	return nStrength;
}

int32 UCardsDeck::DrawCard()
{
	if (cardIndicesInDeck.Num() > 0)
	{
		int32 nPop = cardIndicesInDeck.Pop();
		
		//visual
		GetBoardRenderer()->UpdateDeckCounter();

		return nPop;
	}
	else
	{
		return INVALID_INSTANCE_ID;
	}
}

UCardTemplate* UCardsDeck::GetDeckKingTemplate()
{
	return GetCardManager()->GetCardTemplate(nSelectedKingIndex);
}

void UCardsDeck::GetCardsInDeck(int32 nCardType, int32 nCardEffect, TArray<int32>& cardsInDeckList)
{
	UCardTemplate* cardTemplate = nullptr;
	
	int32 nCounter = 0;
	
	while (nCounter < cardIndicesInDeck.Num())
	{
		cardTemplate = GetCardManager()->GetCardTemplate(cardIndicesInDeck[nCounter]);
		
		if (cardTemplate)
		{
			if ((cardTemplate->IsType(nCardType) || nCardType == CardType_None) 
				&& (cardTemplate->HasEffect(nCardEffect) || nCardEffect == CardEffect_None))
			{
				cardsInDeckList.Add(cardIndicesInDeck[nCounter]);
			}
		}
		else
		{
			LogError("GFX [ERROR] - failed to fetch template reference for card ID: " + FString::FromInt(cardIndicesInDeck[nCounter]));
		}
		
		nCounter++;
	}
}

void UCardsDeck::ShuffleDeck(int32 nStrength)
{
	TArray<int32> tempCardIndices;
	
	int32 nCounter = 0;
	int32 nIndex = 0;
	
	while (nCounter < cardIndices.Num())
	{
		tempCardIndices.Add(cardIndices[nCounter]);
		
		nCounter++;
	}
	
	//AdjustDeckToDifficulty(nStrength, tempCardIndices);

	LogWarning("TODO AdjustDeckToDifficulty");

		
	cardIndicesInDeck.Empty();
	
	while (tempCardIndices.Num() > 0)
	{
		nIndex = FMath::Min(FPlatformMath::FloorToInt(UKismetMathLibrary::RandomFloatInRange(0, 1) * tempCardIndices.Num()), (tempCardIndices.Num() - 1));
		
		cardIndicesInDeck.Add(tempCardIndices[nIndex]);
		
		tempCardIndices.RemoveAt(nIndex);
	}

	//special card?

	//visual
	GetBoardRenderer()->UpdateDeckCounter();
}
