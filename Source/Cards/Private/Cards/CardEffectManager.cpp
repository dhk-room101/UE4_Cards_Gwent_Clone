// Fill out your copyright notice in the Description page of Project Settings.

#include "CardEffectManager.h"
#include "CardManager.h"

#include "ldf.h"

UCardEffectManager::UCardEffectManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

TArray<UCardInstance*> UCardEffectManager::GetEffectsForList(int32 nTargetSlot, int32 nPlayer)
{
	return GetEffectList(nTargetSlot, nPlayer);
}

TArray<UCardInstance*>& UCardEffectManager::GetEffectList(int32 nCardListLocation, int32 nPlayer)
{
	if (nPlayer == PLAYER_1)
	{
		if (nCardListLocation == CARD_LIST_LOC_MELEE)
		{
			return meleeP1EffectsList;
		}
		if (nCardListLocation == CARD_LIST_LOC_RANGED)
		{
			return rangedP1EffectsList;
		}
		if (nCardListLocation == CARD_LIST_LOC_SIEGE)
		{
			return siegeP1EffectsList;
		}
	}
	else if (nPlayer == PLAYER_2)
	{
		if (nCardListLocation == CARD_LIST_LOC_MELEE)
		{
			return meleeP2EffectsList;
		}
		if (nCardListLocation == CARD_LIST_LOC_RANGED)
		{
			return rangedP2EffectsList;
		}
		if (nCardListLocation == CARD_LIST_LOC_SIEGE)
		{
			return siegeP2EffectsList;
		}
	}


	LogError("GetEffectList| Should not be here!!!");



	return *(new TArray<UCardInstance*>());
}

void UCardEffectManager::RegisterActiveEffectCardInstance(UCardInstance * cardInstance, int32 nCardListLocation, int32 nPlayer)
{
	TArray<UCardInstance*>& effectsList = GetEffectList(nCardListLocation, nPlayer);

	LogWarning("GFX - effect was registered in list:" + FString::FromInt(nCardListLocation) + ", for player:" + FString::FromInt(nPlayer) + " and CardInstance:" + cardInstance->templateRef->sTitle);

	effectsList.Add(cardInstance);
	
	//LogError("GFX - Failed to set effect into proper list in GFX manager. listID: " + FString::FromInt(nCardListLocation) + ", playerID: " + FString::FromInt(nPlayer));

	GetCardManager()->RecalculateScores();
}

void UCardEffectManager::UnregisterActiveEffectCardInstance(UCardInstance * cardInstance)
{
	int32 nIndex = 0;
	LogWarning("GFX - unregistering Effect: " + cardInstance->templateRef->sTitle);

	nIndex = siegeP2EffectsList.IndexOfByKey(cardInstance);
	if (nIndex != -1)
	{
		siegeP2EffectsList.RemoveAt(nIndex);
	}

	nIndex = rangedP2EffectsList.IndexOfByKey(cardInstance);
	if (nIndex != -1)
	{
		rangedP2EffectsList.RemoveAt(nIndex);
	}

	nIndex = meleeP2EffectsList.IndexOfByKey(cardInstance);
	if (nIndex != -1)
	{
		meleeP2EffectsList.RemoveAt(nIndex);
	}

	nIndex = meleeP1EffectsList.IndexOfByKey(cardInstance);
	if (nIndex != -1)
	{
		meleeP1EffectsList.RemoveAt(nIndex);
	}

	nIndex = rangedP1EffectsList.IndexOfByKey(cardInstance);
	if (nIndex != -1)
	{
		rangedP1EffectsList.RemoveAt(nIndex);
	}

	nIndex = siegeP1EffectsList.IndexOfByKey(cardInstance);
	if (nIndex != -1)
	{
		siegeP1EffectsList.RemoveAt(nIndex);
	}

	GetCardManager()->RecalculateScores();
}

void UCardEffectManager::FlushAllEffects()
{
	meleeP1EffectsList.Empty();
	meleeP2EffectsList.Empty();

	rangedP1EffectsList.Empty();
	rangedP2EffectsList.Empty();

	siegeP1EffectsList.Empty();
	siegeP2EffectsList.Empty();
}

