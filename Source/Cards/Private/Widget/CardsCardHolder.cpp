// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsCardHolder.h"

#include "CardsGameMain.h"
#include "CardSlot.h"
#include "CardManager.h"
#include "CardInstance.h"

#include "ldf.h"

void UCardsCardHolder::CardAdded(UCardSlot * cardSlot)
{
	RepositionAllCards();
}

void UCardsCardHolder::RepositionAllCards()
{
	if (nCardHolder != CARD_LIST_LOC_GRAVEYARD)
	{
		if (nCardHolder == CARD_LIST_LOC_MELEE
			|| nCardHolder == CARD_LIST_LOC_SIEGE
			|| nCardHolder == CARD_LIST_LOC_RANGED
			|| nCardHolder == CARD_LIST_LOC_HAND)
		{
			RepositionAllCards_Standard(true);
		}
		else
		{
			RepositionAllCards_Standard(false);
		}
	}
	else
	{
		RepositionAllCards_Graveyard();
	}
}

void UCardsCardHolder::RepositionAllCards_Graveyard()
{
	//TODO RepositionAllCards_Graveyard
}

void UCardsCardHolder::RepositionAllCards_Standard(bool bFit)
{
	//TODO bFit, Tween, etc
	for (int32 i = 0; i < GetChildrenCount(); i++)
	{
		if (GetChildAt(i) && GetChildAt(i)->IsA(UCardSlot::StaticClass()))
		{
			UCardSlot* _cardSlot = Cast<UCardSlot>(GetChildAt(i));
			OnPositionEnded(_cardSlot);
		}
	}
}

void UCardsCardHolder::OnPositionEnded(UCardSlot* cardSlot)
{
	UCardInstance* cardInstance = GetCardManager()->GetCardInstance(cardSlot->cardInstanceRef->nInstanceId);
	if (cardInstance)
		cardInstance->OnFinishedMovingIntoHolder(nCardHolder, nPlayer);
}
