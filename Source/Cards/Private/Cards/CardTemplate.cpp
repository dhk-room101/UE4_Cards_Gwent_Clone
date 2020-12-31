// Fill out your copyright notice in the Description page of Project Settings.

#include "CardTemplate.h"

#include "CardManager.h"

#include "CardsCardValues.h"

#include "ldf.h"

UCardTemplate::UCardTemplate(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool UCardTemplate::IsType(int32 nType)
{
	if (nType == CardType_None && nTypeArray != 0)
	{
		return false;
	}
	return (nTypeArray & nType) == nType;
}

bool UCardTemplate::HasEffect(int32 nEffect)
{
	int32 nEffectFlag = 0;
	
	while (nEffectFlag < effectFlags.Num())
	{
		if (effectFlags[nEffectFlag] == nEffect)
		{
			return true;
		}
		
		++nEffectFlag;
	}
	return false;
}

int32 UCardTemplate::GetFirstEffect()
{
	if (effectFlags.Num() == 0)
	{
		return CardEffect_None;
	}
	return effectFlags[0];
}

int32 UCardTemplate::GetPlacementType()
{
	return nTypeArray & CardType_SiegeRangedMelee;
}

int32 UCardTemplate::GetEffectsAsPlacementType()
{
	int32 nPlacement = CardType_None;
	
	if (HasEffect(CardEffect_Melee))
	{
		nPlacement = nPlacement | CardType_Melee;
	}
	
	if (HasEffect(CardEffect_Ranged))
	{
		nPlacement = nPlacement | CardType_Ranged;
	}
	
	if (HasEffect(CardEffect_Siege))
	{
		nPlacement = nPlacement | CardType_Siege;
	}
	
	return nPlacement;
}

int32 UCardTemplate::GetCreatureType()
{
	return nTypeArray & CardType_SiegeRangedMelee;
}

FString UCardTemplate::GetTypeString()
{
	if (IsType(CardType_Row_Modifier))
	{
		return "horn";
	}
	if (IsType(CardType_Weather))
	{
		if (HasEffect(CardEffect_ClearSky) || HasEffect(CardEffect_Clear_Weather))
		{
			return "clearsky";
		}
		if (HasEffect(CardEffect_Melee))
		{
			return "frost";
		}
		if (HasEffect(CardEffect_Ranged))
		{
			return "fog";
		}
		if (HasEffect(CardEffect_Siege))
		{
			return "rain";
		}
	}
	else if (IsType(CardType_Spell))
	{
		if (HasEffect(CardEffect_UnsummonDummy))
		{
			return "dummy";
		}
	}
	else if (IsType(CardType_Global_Effect))
	{
		if (HasEffect(CardEffect_Scorch))
		{
			return "scorch";
		}
	}
	else if (IsType(CardType_Hero))
	{
		return "Hero";
	}
	return GetPlacementTypeString();
}

FString UCardTemplate::GetPlacementTypeString()
{
	if (IsType(CardType_Creature))
	{
		if (IsType(CardType_RangedMelee))
		{
			return "RangedMelee";
		}
		if (IsType(CardType_Melee))
		{
			return "Melee";
		}
		if (IsType(CardType_Ranged))
		{
			return "Ranged";
		}
		if (IsType(CardType_Siege))
		{
			return "Siege";
		}
	}
	return "None";
}

FString UCardTemplate::GetEffectString()
{
	if (IsType(CardType_Creature))
	{
		if (HasEffect(CardEffect_SummonClones))
		{
			return "Muster";
		}
		if (HasEffect(CardEffect_Nurse))
		{
			return "Medic";
		}
		if (HasEffect(CardEffect_Draw2))
		{
			return "Spy";
		}
		if (HasEffect(CardEffect_SameTypeMorale))
		{
			return "Tight Bond";
		}
		if (HasEffect(CardEffect_ImproveNeighbours))
		{
			return "Morale Boost";
		}
		if (HasEffect(CardEffect_Horn))
		{
			return "Commander's Horn";
		}
		if (IsType(CardType_RangedMelee))
		{
			return "Agile";
		}
		if (HasEffect(CardEffect_MeleeScorch))
		{
			return "Scorch";
		}
	}
	return "None";
}

FString UCardTemplate::GetTooltipIcon()
{
	if (IsType(CardType_Row_Modifier))
	{
		return "horn";
	}
	if (IsType(CardType_Weather))
	{
		if (HasEffect(CardEffect_ClearSky) || HasEffect(CardEffect_Clear_Weather))
		{
			return "clearsky";
		}
		if (HasEffect(CardEffect_Melee))
		{
			return "frost";
		}
		if (HasEffect(CardEffect_Ranged))
		{
			return "fog";
		}
		if (HasEffect(CardEffect_Siege))
		{
			return "rain";
		}
	}
	else if (IsType(CardType_Spell))
	{
		if (HasEffect(CardEffect_UnsummonDummy))
		{
			return "dummy";
		}
	}
	else if (IsType(CardType_Global_Effect))
	{
		if (HasEffect(CardEffect_Scorch))
		{
			return "scorch";
		}
	}
	else if (IsType(CardType_Creature))
	{
		if (HasEffect(CardEffect_SummonClones))
		{
			return "summonClones";
		}
		if (HasEffect(CardEffect_Nurse))
		{
			return "nurse";
		}
		if (HasEffect(CardEffect_Draw2))
		{
			return "spy";
		}
		if (HasEffect(CardEffect_SameTypeMorale))
		{
			return "stMorale";
		}
		if (HasEffect(CardEffect_ImproveNeighbours))
		{
			return "impNeighbours";
		}
		if (HasEffect(CardEffect_Horn))
		{
			return "horn";
		}
		if (IsType(CardType_RangedMelee))
		{
			return "agile";
		}
		if (HasEffect(CardEffect_MeleeScorch))
		{
			return "scorch";
		}
	}
	return "None";
}

FString UCardTemplate::GetTooltipString()
{
	if (IsType(CardType_Row_Modifier))
	{
		return "Doubles the strength of all unit cards in a row. Limited to 1 per row.";
	}
	if (IsType(CardType_Weather))
	{
		if (HasEffect(CardEffect_ClearSky) || HasEffect(CardEffect_Clear_Weather))
		{
			return "Removes all Weather Card (Biting Frost, Impenetrable Fog and Torrential Rain) effects.";
		}
		if (HasEffect(CardEffect_Melee))
		{
			return "Sets the strength of all Close Combat cards to 1 for both players.";
		}
		if (HasEffect(CardEffect_Ranged))
		{
			return "Sets the strength of all Ranged Combat cards to 1 for both players.";
		}
		if (HasEffect(CardEffect_Siege))
		{
			return "Sets the strength of all Siege Combat cards to 1 for both players.";
		}
	}
	else if (IsType(CardType_Spell))
	{
		if (HasEffect(CardEffect_UnsummonDummy))
		{
			return "Swap with a card on the battlefield to return it to your hand.";
		}
	}
	else if (IsType(CardType_Global_Effect))
	{
		if (HasEffect(CardEffect_Scorch))
		{
			return "Discard after playing. Kills the strongest card(s) on the battlefield.";
		}
	}
	else if (IsType(CardType_Creature))
	{
		if (HasEffect(CardEffect_SummonClones))
		{
			return "cards_card_tooltip_summon_clones";
		}
		if (HasEffect(CardEffect_Nurse))
		{
			return "Choose one card from your discard pile and play it instantly (no Heroes or Special Cards).";
		}
		if (HasEffect(CardEffect_Draw2))
		{
			return "Place on your opponent's battlefield (count towards opponent's total) and draw 2 cards from your deck.";
		}
		if (HasEffect(CardEffect_SameTypeMorale))
		{
			return "Place next to a card with the same name to double the strength of both cards.";
		}
		if (HasEffect(CardEffect_ImproveNeighbours))
		{
			return "Adds +1 strength to all units in the row, excluding itself.";
		}
		if (HasEffect(CardEffect_Horn))
		{
			return "Doubles the strength of all unit cards in a row. Limited to 1 per row.";
		}
		if (IsType(CardType_RangedMelee))
		{
			return "Can be placed in either the Close Combat or Ranged Combat row. Cannot be moved once placed";
		}
		if (IsType(CardType_Hero))
		{
			return "Not affected by special cards, weather cards or abilities.";
		}
		if (HasEffect(CardEffect_MeleeScorch))
		{
			return "Destroy your enemy's strongest close combat unit(s) if the combined strength of all of his or her combat unit(s) is 10 or more.";
			//return "cards_card_villen_melee_scorch";
		}
	}
	else if (IsType(CardType_None))
	{
		switch (GetFirstEffect())
		{
		case CardEffect_Clear_Weather:
		{
			return "cards_card_tooltip_ldr_clear_weather";
		}
		case CardEffect_Pick_Fog:
		{
			return "Instantly plays Fog Weather card (impacts Ranged rows) from deck.";
		}
		case CardEffect_Siege_Horn:
		{
			return "cards_card_tooltip_ldr_siege_horn";
		}
		case CardEffect_Range_Scorch:
		{
			return "kills the strongest card(s) on the ranged row";
		}
		case CardEffect_Pick_Frost:
		{
			return "Instantly plays Frost Weather card (impacts Melee rows) from deck.";
		}
		case CardEffect_Range_Horn:
		{
			return "cards_card_tooltip_ldr_range_horn";
		}
		case CardEffect_11th_card:
		{
			return "cards_card_tooltip_ldr_eleventh_card";
		}
		case CardEffect_MeleeScorch:
		{
			return "kills the strongest card(s) on the melee row";
		}
		case CardEffect_Pick_Rain:
		{
			return "cards_card_tooltip_ldr_pick_rain";
		}
		case CardEffect_View_3_Enemy:
		{
			return "cards_card_tooltip_ldr_view_enemy";
		}
		case CardEffect_Resurect_Enemy:
		{
			return "cards_card_tooltip_ldr_resurect_enemy";
		}
		case CardEffect_Counter_King:
		{
			return "cards_card_tooltip_ldr_counter_king";
		}
		case CardEffect_Bin2_Pick1:
		{
			return "cards_card_tooltip_ldr_bin_pick";
		}
		case CardEffect_Pick_Weather:
		{
			return "cards_card_tooltip_ldr_pick_weather";
		}
		case CardEffect_Resurect:
		{
			return "cards_card_tooltip_ldr_resurect";
		}
		case CardEffect_Melee_Horn:
		{
			return "cards_card_tooltip_ldr_melee_horn";
		}
		}
	}
	return "";
}

FString UCardTemplate::GetFactionString()
{
	switch (nFactionIdx)
	{
	case FactionId_Neutral:
	{
		return "Neutral";
	}
	case FactionId_No_Mans_Land:
	{
		return "NoMansLand";
	}
	case FactionId_Nilfgaard:
	{
		return "Nilfgaard";
	}
	case FactionId_Northern_Kingdom:
	{
		return "NorthKingdom";
	}
	case FactionId_Scoiatael:
	{
		return "Scoiatael";
	}
	}
	return "None";
}

FString UCardTemplate::ToString()
{
	return "[Cards CardTemplate] index:" + FString::FromInt(nIndex) + ", title:" + sTitle + ", imageLoc:" + sImageLoc + ", power:" + FString::FromInt(nPower) + ", facionIdx:" + FString::FromInt(nFactionIdx) + ", type:" + FString::FromInt(nTypeArray) + ", effectString: " + GetEffectString();
}

float UCardTemplate::GetBonusValue()
{
	int32 nKey = 0;
	int32 nValue = 0;
	
	float fBonus = 0;
	
	TMap<int32, int32> effectsMap = GetCardManager()->cardValues->GetEffectValueDictionary();
	
	while (nKey < effectFlags.Num())
	{
		nValue = effectsMap[effectFlags[nKey]];
		
		if (nValue)
		{
			fBonus = fBonus + nValue;
		}
		
		nKey++;
	}
	
	return fBonus;
}
