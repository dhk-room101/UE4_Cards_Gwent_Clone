#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

#include "CardsPlayerController.h"

class UCardManager;
class UCardsGameFlowController;
class UCardsGameMain;
class UDeckBuilder;
class UCardsGameInstance;

//const
const int32 CARD_ORIGIN_HEIGHT = 584;
const int32 CARD_ORIGIN_WIDTH = 309;
const int32 CARD_BOARD_HEIGHT = 250;
const int32 CARD_BOARD_WIDTH = 187;
const int32 PADDING_X = 4;
const int32 PADDING_Y = 5;

const int32 INVALID_INSTANCE_ID = -1;

//game flow states
const int32 STATE_INVALID = 0;
const int32 STATE_INITIALIZING = 1;
const int32 STATE_TUTORIALS = 2;
const int32 STATE_SPAWNLEADERS = 3;
const int32 STATE_COINTOSS = 4;
const int32 STATE_MULLIGAN = 5;
const int32 STATE_ROUNDSTART = 6;
const int32 STATE_PLAYERTURN = 7;
const int32 STATE_CHANGINGPLAYER = 8;
const int32 STATE_SHOWINGROUNDRESULT = 9;
const int32 STATE_CLEARINGBOARD = 10;
const int32 STATE_SHOWINGFINALRESULT = 11;
const int32 STATE_RESET = 12;
//players states
const int32 STATE_IDLE = 13;
const int32 STATE_CHOOSINGMOVE = 14;
const int32 STATE_CHOOSINGHANDLER = 15;
const int32 STATE_CHOOSINGTARGETCARD = 16;
const int32 STATE_WAITCONFIRMATION = 17;
const int32 STATE_APPLYINGCARD = 18;
const int32 STATE_SENDINGCARDTOTRANSACTION = 19;
const int32 STATE_DELAYBETWEENACTIONS = 20;
//custom
const int32 STATE_ROUNDPASSED = 21;
const int32 STATE_ABILITY_NILFGAARD = 22;
const int32 STATE_ABILITY_NORTHERN = 23;
const int32 STATE_ABILITY_MONSTERS = 24;
const int32 STATE_ABILITY_SKOIATAEL = 25;
const int32 STATE_COINTOSS_OPPONENT = 26;
const int32 STATE_COINTOSS_PLAYER = 27;
const int32 STATE_PLAYERTURN_OPPONENT = 28;
const int32 STATE_PLAYERTURN_PLAYER = 29;
const int32 STATE_ROUNDLOST = 30;
const int32 STATE_ROUNDWON = 31;

const int32 ROUND_PLAYER_STATUS_ACTIVE = 1;
const int32 ROUND_PLAYER_STATUS_DONE = 2;

const int32 PLAYER_INVALID = -1;
const int32 PLAYER_1 = 0;
const int32 PLAYER_2 = 1;
const int32 PLAYER_BOTH = 2;

const int32 CARD_LIST_LOC_INVALID = -1;
const int32 CARD_LIST_LOC_DECK = 0;
const int32 CARD_LIST_LOC_HAND = 1;
const int32 CARD_LIST_LOC_GRAVEYARD = 2;
const int32 CARD_LIST_LOC_SIEGE = 3;
const int32 CARD_LIST_LOC_RANGED = 4;
const int32 CARD_LIST_LOC_MELEE = 5;
const int32 CARD_LIST_LOC_SIEGEMODIFIERS = 6;
const int32 CARD_LIST_LOC_RANGEDMODIFIERS = 7;
const int32 CARD_LIST_LOC_MELEEMODIFIERS = 8;
const int32 CARD_LIST_LOC_WEATHERSLOT = 9;
const int32 CARD_LIST_LOC_LEADER = 10;
const int32 CARD_LIST_LOC_ZOOM = 11;
//DHK
const int32 CARD_LIST_LOC_ALL = 12;
const int32 CARD_LIST_LOC_DECKBUILDER = 13;

const int32 FactionId_Error = -1;
const int32 FactionId_Neutral = 0;
const int32 FactionId_Northern_Kingdom = 1;
const int32 FactionId_Nilfgaard = 2;
const int32 FactionId_No_Mans_Land = 3;
const int32 FactionId_Scoiatael = 4;

const int32 CardType_None = 0;
const int32 CardType_Melee = 1;
const int32 CardType_Ranged = 2;
const int32 CardType_RangedMelee = 3;
const int32 CardType_Siege = 4;
const int32 CardType_SiegeRangedMelee = 7;
const int32 CardType_Creature = 8;
const int32 CardType_Weather = 16;
const int32 CardType_Spell = 32;
const int32 CardType_Row_Modifier = 64;
const int32 CardType_Hero = 128;
const int32 CardType_Spy = 256;
const int32 CardType_Friendly_Effect = 512;
const int32 CardType_Offsensive_Effect = 1024;
const int32 CardType_Global_Effect = 2048;

const int32 CardEffect_None = 0;
const int32 CardEffect_Backstab = 1;
const int32 CardEffect_Morale_Boost = 2;
const int32 CardEffect_Ambush = 3;
const int32 CardEffect_ToughSkin = 4;
const int32 CardEffect_Bin2 = 5;
const int32 CardEffect_Bin3 = 6;
const int32 CardEffect_MeleeScorch = 7;
const int32 CardEffect_11th_card = 8;
const int32 CardEffect_Clear_Weather = 9;
const int32 CardEffect_Pick_Weather = 10;
const int32 CardEffect_Pick_Rain = 11;
const int32 CardEffect_Pick_Fog = 12;
const int32 CardEffect_Pick_Frost = 13;
const int32 CardEffect_View_3_Enemy = 14;
const int32 CardEffect_Resurect = 15;
const int32 CardEffect_Resurect_Enemy = 16;
const int32 CardEffect_Bin2_Pick1 = 17;
const int32 CardEffect_Melee_Horn = 18;
const int32 CardEffect_Range_Horn = 19;
const int32 CardEffect_Siege_Horn = 20;
const int32 CardEffect_Range_Scorch = 21;
const int32 CardEffect_Counter_King = 22;
const int32 CardEffect_Melee = 23;
const int32 CardEffect_Ranged = 24;
const int32 CardEffect_Siege = 25;
const int32 CardEffect_UnsummonDummy = 26;
const int32 CardEffect_Horn = 27;
const int32 CardEffect_Draw = 28;
const int32 CardEffect_Scorch = 29;
const int32 CardEffect_ClearSky = 30;
const int32 CardEffect_SummonClones = 31;
const int32 CardEffect_ImproveNeighbours = 32;
const int32 CardEffect_Nurse = 33;
const int32 CardEffect_Draw2 = 34;
const int32 CardEffect_SameTypeMorale = 35;

const int32 TACTIC_NONE = 0;
const int32 TACTIC_MINIMIZE_LOSS = 1;
const int32 TACTIC_MINIMIZE_WIN = 2;
const int32 TACTIC_MAXIMIZE_WIN = 3;
const int32 TACTIC_AVERAGE_WIN = 4;
const int32 TACTIC_MINIMAL_WIN = 5;
const int32 TACTIC_JUST_WAIT = 6;
const int32 TACTIC_PASS = 7;
const int32 TACTIC_WAIT_DUMMY = 8;
const int32 TACTIC_SPY = 9;
const int32 TACTIC_SPY_DUMMY_BEST_THEN_PASS = 10;

const int32 SortType_None = 0;
const int32 SortType_StrategicValue = 1;
const int32 SortType_PowerChange = 2;

void LogWarning(FString sWarning);
void LogError(FString sError);

UWorld* GetCurrentWorld();

ACardsPlayerController* GetCurrentPlayerController();

UCardsGameMain* GetBoardRenderer();

UDeckBuilder* GetDeckBuilder();

UCardManager* GetCardManager();

UCardsGameFlowController* GetGameFlowController();

UCardsGameInstance* GetCurrentGameInstance();

int32 ConvertTypeStringToInt(FString sType);

int32 ConvertFactionStringToInt(FString sFaction);

int32 ConvertEffectStringToInt(FString sEffect);

FString ConvertAttitudeToString(int32 nAttitude);

FString ConvertFactionToString(int32 nFactionIdx);

FString ConvertStateToString(int32 nState);