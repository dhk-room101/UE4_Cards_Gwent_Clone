// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsPlayerController.h"
#include "CardsGameInstance.h"

#include "GameResults.h"
#include "DeckBuilder.h"
#include "CardsGameMain.h"
#include "CardSlot.h"

#include "ldf.h"

#include "CardsLibrary.h"
#include "Sound/SoundCue.h"
#include "ConstructorHelpers.h"
#include "Engine/Texture2D.h"

ACardsPlayerController::ACardsPlayerController()
{
	bShowMouseCursor = true;
	//DefaultMouseCursor = EMouseCursor::Crosshairs;

	LoadTextures();
	LoadSounds();
}

void ACardsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	InitializeUI();

	//when spawn, update the singleton world to match current
	bool IsValid;

	if (GetCurrentWorld() != GetWorld())
	{
		UCardsLibrary::GetCardsData(IsValid)->CardsCurrentWorld = GetWorld();

		LogWarning("updating world in singleton to " + GetWorld()->GetName());

	}

	//Cast<UCardsGameInstance>(GetGameInstance())->StartGame();
	if (UIDeckBuilder)
		UIDeckBuilder->StartDeckBuilder();
}

void ACardsPlayerController::InitializeUI()
{
	if (wUIDeckBuilder)
	{
		UIDeckBuilder = CreateWidget<UDeckBuilder>(this, wUIDeckBuilder);

		if (UIDeckBuilder)
		{
			UIDeckBuilder->AddToViewport();
			UIDeckBuilder->SetVisibility(ESlateVisibility::Visible);
		}
	}

	if (wUICardsGameMain)
	{
		UICardsGameMain = CreateWidget<UCardsGameMain>(this, wUICardsGameMain);

		if (UICardsGameMain)
		{
			UICardsGameMain->AddToViewport();
			UICardsGameMain->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (wUIGameResults)
	{
		UIGameResults = CreateWidget<UGameResults>(this, wUIGameResults);

		if (UIGameResults)
		{
			UIGameResults->AddToViewport();
			UIGameResults->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ACardsPlayerController::LoadTextures()
{
	//states textures and such
	static ConstructorHelpers::FObjectFinder<UTexture2D> AbilityNilfgaard(TEXT("/Game/Images/Board/AbilityNilfgaard"));
	statesTextures.Add(STATE_ABILITY_NILFGAARD, AbilityNilfgaard.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> AbilityNorthern(TEXT("/Game/Images/Board/AbilityNorthern"));
	statesTextures.Add(STATE_ABILITY_NORTHERN, AbilityNorthern.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> CoinTossOpponent(TEXT("/Game/Images/Board/CoinTossOpponent"));
	statesTextures.Add(STATE_COINTOSS_OPPONENT, CoinTossOpponent.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> CoinTossPlayer(TEXT("/Game/Images/Board/CoinTossPlayer"));
	statesTextures.Add(STATE_COINTOSS_PLAYER, CoinTossPlayer.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> PlayerTurnOpponent(TEXT("/Game/Images/Board/PlayerTurnOpponent"));
	statesTextures.Add(STATE_PLAYERTURN_OPPONENT, PlayerTurnOpponent.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> PlayerTurnPlayer(TEXT("/Game/Images/Board/PlayerTurnPlayer"));
	statesTextures.Add(STATE_PLAYERTURN_PLAYER, PlayerTurnPlayer.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> RoundLost(TEXT("/Game/Images/Board/RoundLost"));
	statesTextures.Add(STATE_ROUNDLOST, RoundLost.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> RoundPassed(TEXT("/Game/Images/Board/RoundPassed"));
	statesTextures.Add(STATE_ROUNDPASSED, RoundPassed.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> RoundStart(TEXT("/Game/Images/Board/RoundStart"));
	statesTextures.Add(STATE_ROUNDSTART, RoundStart.Object);
	static ConstructorHelpers::FObjectFinder<UTexture2D> RoundWon(TEXT("/Game/Images/Board/RoundWon"));
	statesTextures.Add(STATE_ROUNDWON, RoundWon.Object);

	//effect textures
	static ConstructorHelpers::FObjectFinder<UTexture2D> clear(TEXT("/Game/Images/Icons/icon_clear"));
	effectTextures.Add(CardEffect_ClearSky, clear.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> clones(TEXT("/Game/Images/Icons/icon_clones"));
	effectTextures.Add(CardEffect_SummonClones, clones.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> dummy(TEXT("/Game/Images/Icons/icon_dummy"));
	effectTextures.Add(CardEffect_UnsummonDummy, dummy.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> fog(TEXT("/Game/Images/Icons/icon_fog"));
	effectTextures.Add(CardEffect_Ranged, fog.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> frost(TEXT("/Game/Images/Icons/icon_frost"));
	effectTextures.Add(CardEffect_Melee, frost.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> horn(TEXT("/Game/Images/Icons/icon_horn"));
	effectTextures.Add(CardEffect_Horn, horn.Object);
	effectTextures.Add(CardEffect_Melee_Horn, horn.Object);
	effectTextures.Add(CardEffect_Range_Horn, horn.Object);
	effectTextures.Add(CardEffect_Siege_Horn, horn.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neighbours(TEXT("/Game/Images/Icons/icon_neighbours"));
	effectTextures.Add(CardEffect_ImproveNeighbours, neighbours.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> nurse(TEXT("/Game/Images/Icons/icon_nurse"));
	effectTextures.Add(CardEffect_Nurse, nurse.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> morale(TEXT("/Game/Images/Icons/icon_morale"));
	effectTextures.Add(CardEffect_SameTypeMorale, morale.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> rain(TEXT("/Game/Images/Icons/icon_rain"));
	effectTextures.Add(CardEffect_Siege, rain.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> scorch(TEXT("/Game/Images/Icons/icon_scorch"));
	effectTextures.Add(CardEffect_Scorch, scorch.Object);
	effectTextures.Add(CardEffect_MeleeScorch, scorch.Object);
	effectTextures.Add(CardEffect_Range_Scorch, scorch.Object);
	//no range scorch

	static ConstructorHelpers::FObjectFinder<UTexture2D> spy(TEXT("/Game/Images/Icons/icon_spy"));
	effectTextures.Add(CardEffect_Draw2, spy.Object);

	//type textures
	static ConstructorHelpers::FObjectFinder<UTexture2D> creature(TEXT("/Game/Images/Icons/icon_creature"));
	typeTextures.Add(CardType_Creature, creature.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> hero(TEXT("/Game/Images/Icons/icon_hero"));
	typeTextures.Add(CardType_Hero, hero.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> melee(TEXT("/Game/Images/Icons/icon_melee"));
	typeTextures.Add(CardType_Melee, melee.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> melee_ranged(TEXT("/Game/Images/Icons/icon_melee_ranged"));
	typeTextures.Add(CardType_RangedMelee, melee_ranged.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> ranged(TEXT("/Game/Images/Icons/icon_ranged"));
	typeTextures.Add(CardType_Ranged, ranged.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> siege(TEXT("/Game/Images/Icons/icon_siege"));
	typeTextures.Add(CardType_Siege, siege.Object);

	//faction deck textures
	static ConstructorHelpers::FObjectFinder<UTexture2D> paragon1(TEXT("/Game/Images/Decks/deck_faction1"));
	factionDeckTextures.Add(FactionId_Northern_Kingdom, paragon1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> paragon2(TEXT("/Game/Images/Decks/deck_faction2"));
	factionDeckTextures.Add(FactionId_Nilfgaard, paragon2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> monstersDeck(TEXT("/Game/Images/Decks/deck_monsters"));
	factionDeckTextures.Add(FactionId_No_Mans_Land, monstersDeck.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> scoiataelDeck(TEXT("/Game/Images/Decks/deck_scoiatael"));
	factionDeckTextures.Add(FactionId_Scoiatael, scoiataelDeck.Object);

	//faction banner textures
	static ConstructorHelpers::FObjectFinder<UTexture2D> monstersBanner(TEXT("/Game/Images/Banners/banner_monsters"));
	factionBannerTextures.Add(FactionId_No_Mans_Land, monstersBanner.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> nilfgaardBanner(TEXT("/Game/Images/Banners/banner_nilfgaard"));
	factionBannerTextures.Add(FactionId_Nilfgaard, nilfgaardBanner.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> northernBanner(TEXT("/Game/Images/Banners/banner_northern"));
	factionBannerTextures.Add(FactionId_Northern_Kingdom, northernBanner.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> scoiataelBanner(TEXT("/Game/Images/Banners/banner_scoiatael"));
	factionBannerTextures.Add(FactionId_Scoiatael, scoiataelBanner.Object);

	//pictures
	//specials and neutrals
	static ConstructorHelpers::FObjectFinder<UTexture2D> spc_dummy(TEXT("/Game/Images/Cards/spc_dummy"));
	picTextures.Add(0, spc_dummy.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> spc_horn(TEXT("/Game/Images/Cards/spc_horn"));
	picTextures.Add(1, spc_horn.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> spc_scorch(TEXT("/Game/Images/Cards/spc_scorch"));
	picTextures.Add(2, spc_scorch.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> spc_frost(TEXT("/Game/Images/Cards/spc_frost"));
	picTextures.Add(3, spc_frost.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> spc_fog(TEXT("/Game/Images/Cards/spc_fog"));
	picTextures.Add(4, spc_fog.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> spc_rain(TEXT("/Game/Images/Cards/spc_rain"));
	picTextures.Add(5, spc_rain.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> spc_clearsky(TEXT("/Game/Images/Cards/spc_clearsky"));
	picTextures.Add(6, spc_clearsky.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_geralt(TEXT("/Game/Images/Cards/neu_geralt"));
	picTextures.Add(7, neu_geralt.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_vesemir(TEXT("/Game/Images/Cards/neu_vesemir"));
	picTextures.Add(8, neu_vesemir.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_yennefer(TEXT("/Game/Images/Cards/neu_yennefer"));
	picTextures.Add(9, neu_yennefer.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_ciri(TEXT("/Game/Images/Cards/neu_ciri"));
	picTextures.Add(10, neu_ciri.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_triss(TEXT("/Game/Images/Cards/neu_triss"));
	picTextures.Add(11, neu_triss.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_dandelion(TEXT("/Game/Images/Cards/neu_dandelion"));
	picTextures.Add(12, neu_dandelion.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_zoltan(TEXT("/Game/Images/Cards/neu_zoltan"));
	picTextures.Add(13, neu_zoltan.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_emiel(TEXT("/Game/Images/Cards/neu_emiel"));
	picTextures.Add(14, neu_emiel.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_villen(TEXT("/Game/Images/Cards/neu_villen"));
	picTextures.Add(15, neu_villen.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> neu_avallach(TEXT("/Game/Images/Cards/neu_avallach"));
	picTextures.Add(16, neu_avallach.Object);

	//paragon1
	static ConstructorHelpers::FObjectFinder<UTexture2D> aurora1(TEXT("/Game/Images/Cards/aurora1"));
	picTextures.Add(100, aurora1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> countess1(TEXT("/Game/Images/Cards/countess1"));
	picTextures.Add(101, countess1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> crunch1(TEXT("/Game/Images/Cards/crunch1"));
	picTextures.Add(102, crunch1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> dekker1(TEXT("/Game/Images/Cards/dekker1"));
	picTextures.Add(103, dekker1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> drongo1(TEXT("/Game/Images/Cards/drongo1"));
	picTextures.Add(105, drongo1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> fengmao1(TEXT("/Game/Images/Cards/fengmao1"));
	picTextures.Add(106, fengmao1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> gadget1(TEXT("/Game/Images/Cards/gadget1"));
	picTextures.Add(107, gadget1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> gideon1(TEXT("/Game/Images/Cards/gideon1"));
	picTextures.Add(108, gideon1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> greystone1(TEXT("/Game/Images/Cards/greystone1"));
	picTextures.Add(109, greystone1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> grim1(TEXT("/Game/Images/Cards/grim1"));
	picTextures.Add(111, grim1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> grux1(TEXT("/Game/Images/Cards/grux1"));
	picTextures.Add(112, grux1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> howitzer1(TEXT("/Game/Images/Cards/howitzer1"));
	picTextures.Add(113, howitzer1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> iggy1(TEXT("/Game/Images/Cards/iggy1"));
	picTextures.Add(114, iggy1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> kallari1(TEXT("/Game/Images/Cards/kallari1"));
	picTextures.Add(115, kallari1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> khaimera1(TEXT("/Game/Images/Cards/khaimera1"));
	picTextures.Add(116, khaimera1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> kwang1(TEXT("/Game/Images/Cards/kwang1"));
	picTextures.Add(120, kwang1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> morigesh1(TEXT("/Game/Images/Cards/morigesh1"));
	picTextures.Add(121, morigesh1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> ltbelica1(TEXT("/Game/Images/Cards/ltbelica1"));
	picTextures.Add(125, ltbelica1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> murdock1(TEXT("/Game/Images/Cards/murdock1"));
	picTextures.Add(126, murdock1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> muriel1(TEXT("/Game/Images/Cards/muriel1"));
	picTextures.Add(127, muriel1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> narbash1(TEXT("/Game/Images/Cards/narbash1"));
	picTextures.Add(130, narbash1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> rampage1(TEXT("/Game/Images/Cards/rampage1"));
	picTextures.Add(131, rampage1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> revenant1(TEXT("/Game/Images/Cards/revenant1"));
	picTextures.Add(132, revenant1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> sevarog1(TEXT("/Game/Images/Cards/sevarog1"));
	picTextures.Add(135, sevarog1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> riktor1(TEXT("/Game/Images/Cards/riktor1"));
	picTextures.Add(136, riktor1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> serath1(TEXT("/Game/Images/Cards/serath1"));
	picTextures.Add(140, serath1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> phase1(TEXT("/Game/Images/Cards/phase1"));
	picTextures.Add(141, phase1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> shinbi1(TEXT("/Game/Images/Cards/shinbi1"));
	picTextures.Add(145, shinbi1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> sparrow1(TEXT("/Game/Images/Cards/sparrow1"));
	picTextures.Add(146, sparrow1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> steel1(TEXT("/Game/Images/Cards/steel1"));
	picTextures.Add(150, steel1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> terra1(TEXT("/Game/Images/Cards/terra1"));
	picTextures.Add(151, terra1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> fey1(TEXT("/Game/Images/Cards/fey1"));
	picTextures.Add(152, fey1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> wukong1(TEXT("/Game/Images/Cards/wukong1"));
	picTextures.Add(160, wukong1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> yin1(TEXT("/Game/Images/Cards/yin1"));
	picTextures.Add(161, yin1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> zinx1(TEXT("/Game/Images/Cards/zinx1"));
	picTextures.Add(162, zinx1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> twinblast1(TEXT("/Game/Images/Cards/twinblast1"));
	picTextures.Add(170, twinblast1.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> wraith1(TEXT("/Game/Images/Cards/wraith1"));
	picTextures.Add(175, wraith1.Object);

	//paragon2
	static ConstructorHelpers::FObjectFinder<UTexture2D> aurora2(TEXT("/Game/Images/Cards/aurora2"));
	picTextures.Add(200, aurora2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> countess2(TEXT("/Game/Images/Cards/countess2"));
	picTextures.Add(201, countess2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> crunch2(TEXT("/Game/Images/Cards/crunch2"));
	picTextures.Add(202, crunch2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> dekker2(TEXT("/Game/Images/Cards/dekker2"));
	picTextures.Add(203, dekker2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> drongo2(TEXT("/Game/Images/Cards/drongo2"));
	picTextures.Add(205, drongo2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> fengmao2(TEXT("/Game/Images/Cards/fengmao2"));
	picTextures.Add(206, fengmao2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> gadget2(TEXT("/Game/Images/Cards/gadget2"));
	picTextures.Add(207, gadget2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> gideon2(TEXT("/Game/Images/Cards/gideon2"));
	picTextures.Add(208, gideon2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> greystone2(TEXT("/Game/Images/Cards/greystone2"));
	picTextures.Add(209, greystone2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> grim2(TEXT("/Game/Images/Cards/grim2"));
	picTextures.Add(210, grim2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> grux2(TEXT("/Game/Images/Cards/grux2"));
	picTextures.Add(211, grux2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> howitzer2(TEXT("/Game/Images/Cards/howitzer2"));
	picTextures.Add(212, howitzer2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> iggy2(TEXT("/Game/Images/Cards/iggy2"));
	picTextures.Add(213, iggy2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> kallari2(TEXT("/Game/Images/Cards/kallari2"));
	picTextures.Add(214, kallari2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> khaimera2(TEXT("/Game/Images/Cards/khaimera2"));
	picTextures.Add(215, khaimera2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> kwang2(TEXT("/Game/Images/Cards/kwang2"));
	picTextures.Add(217, kwang2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> morigesh2(TEXT("/Game/Images/Cards/morigesh2"));
	picTextures.Add(218, morigesh2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> ltbelica2(TEXT("/Game/Images/Cards/ltbelica2"));
	picTextures.Add(219, ltbelica2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> murdock2(TEXT("/Game/Images/Cards/murdock2"));
	picTextures.Add(220, murdock2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> muriel2(TEXT("/Game/Images/Cards/muriel2"));
	picTextures.Add(221, muriel2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> narbash2(TEXT("/Game/Images/Cards/narbash2"));
	picTextures.Add(230, narbash2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> rampage2(TEXT("/Game/Images/Cards/rampage2"));
	picTextures.Add(231, rampage2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> revenant2(TEXT("/Game/Images/Cards/revenant2"));
	picTextures.Add(235, revenant2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> sevarog2(TEXT("/Game/Images/Cards/sevarog2"));
	picTextures.Add(236, sevarog2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> riktor2(TEXT("/Game/Images/Cards/riktor2"));
	picTextures.Add(240, riktor2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> serath2(TEXT("/Game/Images/Cards/serath2"));
	picTextures.Add(241, serath2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> phase2(TEXT("/Game/Images/Cards/phase2"));
	picTextures.Add(245, phase2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> shinbi2(TEXT("/Game/Images/Cards/shinbi2"));
	picTextures.Add(246, shinbi2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> sparrow2(TEXT("/Game/Images/Cards/sparrow2"));
	picTextures.Add(247, sparrow2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> steel2(TEXT("/Game/Images/Cards/steel2"));
	picTextures.Add(248, steel2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> terra2(TEXT("/Game/Images/Cards/terra2"));
	picTextures.Add(250, terra2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> fey2(TEXT("/Game/Images/Cards/fey2"));
	picTextures.Add(251, fey2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> wukong2(TEXT("/Game/Images/Cards/wukong2"));
	picTextures.Add(252, wukong2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> yin2(TEXT("/Game/Images/Cards/yin2"));
	picTextures.Add(255, yin2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> zinx2(TEXT("/Game/Images/Cards/zinx2"));
	picTextures.Add(260, zinx2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> twinblast2(TEXT("/Game/Images/Cards/twinblast2"));
	picTextures.Add(261, twinblast2.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> wraith2(TEXT("/Game/Images/Cards/wraith2"));
	picTextures.Add(265, wraith2.Object);

	//leaders
	static ConstructorHelpers::FObjectFinder<UTexture2D> aurora_leader(TEXT("/Game/Images/Cards/aurora_leader"));
	picTextures.Add(1001, aurora_leader.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> nor_ldr_foltest_bronze(TEXT("/Game/Images/Cards/nor_ldr_foltest_bronze"));
	picTextures.Add(1002, nor_ldr_foltest_bronze.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> nor_ldr_foltest_silver(TEXT("/Game/Images/Cards/nor_ldr_foltest_silver"));
	picTextures.Add(1003, nor_ldr_foltest_silver.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> nor_ldr_foltest_gold(TEXT("/Game/Images/Cards/nor_ldr_foltest_gold"));
	picTextures.Add(1004, nor_ldr_foltest_gold.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> murdock_leader(TEXT("/Game/Images/Cards/murdock_leader"));
	picTextures.Add(2001, murdock_leader.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> nil_ldr_emhyr_bronze(TEXT("/Game/Images/Cards/nil_ldr_emhyr_bronze"));
	picTextures.Add(2002, nil_ldr_emhyr_bronze.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> nil_ldr_emhyr_silver(TEXT("/Game/Images/Cards/nil_ldr_emhyr_silver"));
	picTextures.Add(2003, nil_ldr_emhyr_silver.Object);

	static ConstructorHelpers::FObjectFinder<UTexture2D> nil_ldr_emhyr_gold(TEXT("/Game/Images/Cards/nil_ldr_emhyr_gold"));
	picTextures.Add(2004, nil_ldr_emhyr_gold.Object);
}

void ACardsPlayerController::LoadSounds()
{
	static ConstructorHelpers::FObjectFinder<USoundCue> ClearWeather(TEXT("/Game/Sounds/ClearWeather_Cue"));
	sounds.Add("ClearWeather", ClearWeather.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> CoinToss(TEXT("/Game/Sounds/CoinToss_Cue"));
	sounds.Add("CoinToss", CoinToss.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> DealCard(TEXT("/Game/Sounds/DealCard_Cue"));
	sounds.Add("DealCard", DealCard.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> GameLost(TEXT("/Game/Sounds/GameLost_Cue"));
	sounds.Add("GameLost", GameLost.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> Melee(TEXT("/Game/Sounds/Melee_Cue"));
	sounds.Add("Melee", Melee.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> PlayerTurn(TEXT("/Game/Sounds/PlayerTurn_Cue"));
	sounds.Add("PlayerTurn", PlayerTurn.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> Ranged(TEXT("/Game/Sounds/Ranged_Cue"));
	sounds.Add("Ranged", Ranged.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> RoundLost(TEXT("/Game/Sounds/RoundLost_Cue"));
	sounds.Add("RoundLost", RoundLost.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> RoundStart(TEXT("/Game/Sounds/RoundStart_Cue"));
	sounds.Add("RoundStart", RoundStart.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> RoundWon(TEXT("/Game/Sounds/RoundWon_Cue"));
	sounds.Add("RoundWon", RoundWon.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> ShuffleCards(TEXT("/Game/Sounds/ShuffleCards_Cue"));
	sounds.Add("ShuffleCards", ShuffleCards.Object);

	static ConstructorHelpers::FObjectFinder<USoundCue> Siege(TEXT("/Game/Sounds/Siege_Cue"));
	sounds.Add("Siege", Siege.Object);

	// 	StartDragUISound = LoadObject<USoundCue>(nullptr, TEXT("/Engine/VREditor/Sounds/VR_negative_Cue"));
	// 	check(StartDragUISound != nullptr);
	//	UGameplayStatics::PlaySound2D( GetOwner().GetWorld(), StartDragUISound );
}
