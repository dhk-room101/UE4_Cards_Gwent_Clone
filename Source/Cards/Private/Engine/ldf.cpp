#include "ldf.h"
#include "CardsLibrary.h"
#include "CardsGameInstance.h"
#include "CardsPlayerController.h"
#include "CardsSingleton.h"
#include "DeckBuilder.h"
#include "Engine/Engine.h"

void LogWarning(FString sWarning)
{
	if (sWarning.Contains("TODO") || sWarning.Contains("[["))
	{
		if (GEngine)
		{
			FColor fColor;

			if (sWarning.Contains("[[PLAYER")) fColor = FColor::Green;
			else if (sWarning.Contains("[[AI")) fColor = FColor::Yellow;
			else if (sWarning.Contains("[[BASE")) fColor = FColor::Cyan;
			else if (sWarning.Contains("[[FSM")) fColor = FColor::Magenta;
			

			if (GetCurrentPlayerController()->LAST_MESSAGE != sWarning)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5, fColor, sWarning);
				GetCurrentPlayerController()->LAST_MESSAGE = sWarning;
			}
		}
	}

	if (GetCurrentWorld())
	{
		FString sTime = FString::SanitizeFloat(GetCurrentWorld()->GetTimeSeconds()) + ") ";
		sWarning = sTime + sWarning;
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *sWarning);
}

void LogError(FString sError)
{
	if (sError.Contains("TODO") || sError.Contains("[["))
	{
		if (GEngine)
		{
			if (GetCurrentPlayerController()->LAST_MESSAGE != sError)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sError);
				GetCurrentPlayerController()->LAST_MESSAGE = sError;
			}
		}
	}

	if (GetCurrentWorld())
	{
		FString sTime = FString::SanitizeFloat(GetCurrentWorld()->GetTimeSeconds()) + ") ";
		sError = sTime + sError;
	}
	UE_LOG(LogTemp, Error, TEXT("%s"), *sError);
}

UWorld* GetCurrentWorld()
{
	bool IsValid;
	return UCardsLibrary::GetCardsData(IsValid)->CardsCurrentWorld;
}

ACardsPlayerController* GetCurrentPlayerController()
{
	return (Cast<ACardsPlayerController>(GetCurrentWorld()->GetFirstPlayerController()));
}

UCardsGameMain* GetBoardRenderer()
{
	return GetCurrentPlayerController()->UICardsGameMain;
}

UDeckBuilder* GetDeckBuilder()
{
	return GetCurrentPlayerController()->UIDeckBuilder;
}

UCardManager* GetCardManager()
{
	if (!GetCurrentWorld() || !GetCurrentPlayerController())
		return nullptr;

	if (GetCurrentPlayerController()->IsDeckBuilder)
		return GetDeckBuilder()->cardManager;
	else
		return GetCurrentGameInstance()->cardManager;

	return nullptr;
}

UCardsGameFlowController* GetGameFlowController()
{
	return GetCurrentWorld() ? GetCurrentGameInstance()->gameFlowController : nullptr;
}

UCardsGameInstance* GetCurrentGameInstance()
{
	return GetCurrentWorld() ? Cast<UCardsGameInstance>(GetCurrentWorld()->GetGameInstance()) : nullptr;
}

int32 ConvertTypeStringToInt(FString sType)
{
	if (sType == "TYPE_MELEE")
	{
		return CardType_Melee;
	}
	else if (sType == "TYPE_RANGED")
	{
		return CardType_Ranged;
	}
	else if (sType == "TYPE_SIEGE")
	{
		return CardType_Siege;
	}
	else if (sType == "TYPE_CREATURE")
	{
		return CardType_Creature;
	}
	else if (sType == "TYPE_WEATHER")
	{
		return CardType_Weather;
	}
	else if (sType == "TYPE_SPELL")
	{
		return CardType_Spell;
	}
	else if (sType == "TYPE_ROW_MODIFIER")
	{
		return CardType_Row_Modifier;
	}
	else if (sType == "TYPE_HERO")
	{
		return CardType_Hero;
	}
	else if (sType == "TYPE_SPY")
	{
		return CardType_Spy;
	}
	else if (sType == "TYPE_FRIENDLY_EFFECT")
	{
		return CardType_Friendly_Effect;
	}
	else if (sType == "TYPE_GLOBAL_EFFECT")
	{
		return CardType_Global_Effect;
	}
	else
	{

		LogError("needs a string that has a return type!");



		return CardType_None;
	}
}

int32 ConvertFactionStringToInt(FString sFaction)
{
	if (sFaction == "F_NEUTRAL")	return FactionId_Neutral;
	//else if (sFaction == "F_NO_MANS_LAND")	return FactionId_No_Mans_Land;
	else if (sFaction == "F_PARAGON1")	return FactionId_Northern_Kingdom;
	else if (sFaction == "F_PARAGON2")	return FactionId_Nilfgaard;
	//else if (sFaction == "F_SCOIATAEL")	return FactionId_Scoiatael;
	else
	{

		LogError("needs a string that has a return faction!");


		return FactionId_Error;
	}
}

int32 ConvertEffectStringToInt(FString sEffect)
{
	if (sEffect == "EFFECT_NONE")	return CardEffect_None;
	else if (sEffect == "EFFECT_MELEE")	return CardEffect_Melee;
	else if (sEffect == "EFFECT_RANGED")	return CardEffect_Ranged;
	else if (sEffect == "EFFECT_SIEGE")	return CardEffect_Siege;
	else if (sEffect == "EFFECT_UNSUMMON_DUMMY")	return CardEffect_UnsummonDummy;
	else if (sEffect == "EFFECT_HORN")	return CardEffect_Horn;
	else if (sEffect == "EFFECT_SCORCH")	return CardEffect_Scorch;
	else if (sEffect == "EFFECT_CLEAR_SKY")	return CardEffect_ClearSky;
	else if (sEffect == "EFFECT_SUMMON_CLONES")	return CardEffect_SummonClones;
	else if (sEffect == "EFFECT_IMPROVE_NEIGHBOURS")	return CardEffect_ImproveNeighbours;
	else if (sEffect == "EFFECT_NURSE")	return CardEffect_Nurse;
	else if (sEffect == "EFFECT_DRAW_X2")	return CardEffect_Draw2;
	else if (sEffect == "EFFECT_SAME_TYPE_MORALE")	return CardEffect_SameTypeMorale;
	else if (sEffect == "CP_11TH_CARD")	return CardEffect_11th_card;
	else if (sEffect == "CP_BIN2_PICK1")	return CardEffect_Bin2_Pick1;
	else if (sEffect == "CP_CLEAR_WEATHER")	return CardEffect_Clear_Weather;
	else if (sEffect == "CP_COUNTER_KING_ABILITY")	return CardEffect_Counter_King;
	else if (sEffect == "CP_MELEE_HORN")	return CardEffect_Melee_Horn;
	else if (sEffect == "CP_MELEE_SCORCH")	return CardEffect_MeleeScorch;
	else if (sEffect == "CP_PICK_FOG_CARD")	return CardEffect_Pick_Fog;
	else if (sEffect == "CP_PICK_FROST_CARD")	return CardEffect_Pick_Frost;
	else if (sEffect == "CP_PICK_RAIN_CARD")	return CardEffect_Pick_Rain;
	else if (sEffect == "CP_PICK_WEATHER_CARD")	return CardEffect_Pick_Weather;
	else if (sEffect == "CP_RANGE_HORN")	return CardEffect_Range_Horn;
	else if (sEffect == "CP_RESURECT_CARD")	return CardEffect_Resurect;
	else if (sEffect == "CP_RESURECT_FROM_ENEMY")	return CardEffect_Resurect_Enemy;
	else if (sEffect == "CP_SIEGE_HORN")	return CardEffect_Siege_Horn;
	else if (sEffect == "CP_RANGE_SCORCH")	return CardEffect_Range_Scorch;
	else if (sEffect == "CP_VIEW_3_ENEMY_CARDS")	return CardEffect_View_3_Enemy;
	else
	{

		LogError("needs a string that has a return effect! Got " + sEffect);


		return CardEffect_None;
	}
}

FString ConvertAttitudeToString(int32 nAttitude)
{
	switch (nAttitude)
	{
	case TACTIC_NONE:	return "NONE - ERROR";
	case TACTIC_SPY_DUMMY_BEST_THEN_PASS:	return "DUMMY BETS THEN PASS";
	case TACTIC_MINIMIZE_LOSS:	return "MINIMIZE LOSS";
	case TACTIC_MINIMIZE_WIN:	return "MINIMIZE WIN";
	case TACTIC_MAXIMIZE_WIN:	return "MAXIMIZE WIN";
	case TACTIC_AVERAGE_WIN:	return "AVERAGE WIN";
	case TACTIC_MINIMAL_WIN:	return "MINIMAL WIN";
	case TACTIC_JUST_WAIT:	return "JUST WAIT";
	case TACTIC_PASS:	return "PASS";
	case TACTIC_WAIT_DUMMY:	return "WAIT DUMMY";
	case TACTIC_SPY:	return "SPIES";
	default:
	{
		break;
	}
	}

	LogWarning("ConvertAttitudeToString: Unknown attitude " + FString::FromInt(nAttitude));


	return FString::FromInt(nAttitude);
}

FString ConvertFactionToString(int32 nFactionIdx)
{
	switch (nFactionIdx)
	{
	case FactionId_Neutral:
	{
		return "Neutral";
	}
	case FactionId_No_Mans_Land:
	{
		return "Monsters";
	}
	case FactionId_Nilfgaard:
	{
		return "Nilfgaardian Empire";
	}
	case FactionId_Northern_Kingdom:
	{
		return "Northern Realms";
	}
	case FactionId_Scoiatael:
	{
		return "Scoia'tael";
	}
	}
	return "None";
}

FString ConvertStateToString(int32 nState)
{
	switch (nState)
	{
	case STATE_INVALID: return "Invalid";
	case STATE_INITIALIZING: return "Initializing";
	case STATE_TUTORIALS: return "Tutorials";
	case STATE_SPAWNLEADERS: return "SpawnLeaders";
	case STATE_COINTOSS: return "CoinToss";
	case STATE_MULLIGAN: return "Mulligan";
	case STATE_ROUNDSTART: return "RoundStart";
	case STATE_PLAYERTURN: return "PlayerTurn";
	case STATE_CHANGINGPLAYER: return "ChangingPlayer";
	case STATE_SHOWINGROUNDRESULT: return "ShowingRoundResult";
	case STATE_CLEARINGBOARD: return "ClearingBoard";
	case STATE_SHOWINGFINALRESULT: return "ShowingFinalResult";
	case STATE_RESET: return "Reset";
	case STATE_IDLE: return "Idle";
	case STATE_CHOOSINGMOVE: return "ChoosingMove";
	case STATE_CHOOSINGHANDLER: return "ChoosingHandler";
	case STATE_CHOOSINGTARGETCARD: return "ChoosingTargetCard";
	case STATE_WAITCONFIRMATION: return "WaitConfirmation";
	case STATE_APPLYINGCARD: return "ApplyingCard";
	case STATE_SENDINGCARDTOTRANSACTION: return "SendingCardToTransaction";
	case STATE_DELAYBETWEENACTIONS: return "DelayBetweenActions";
	default:
		return "Invalid";
	}

	return "Invalid";
}
