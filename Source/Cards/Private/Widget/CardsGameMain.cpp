// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsGameMain.h"

#include "CardSlot.h"
#include "CardInstance.h"
#include "CardLeaderInstance.h"
#include "CardManager.h"
#include "CardDescription.h"
#include "CardCounter.h"
#include "CardsCardHolder.h"
#include "CardsGameFlowController.h"
#include "FiniteStateMachine.h"
#include "HumanPlayerController.h"

#include "ldf.h"

#include "UMG.h"
#include "WidgetBlueprintLibrary.h"

#include "WidgetLayoutLibrary.h"
#include "Engine/Engine.h"

void UCardsGameMain::NativeConstruct()
{
	Super::NativeConstruct();

	Weather = (UCardsCardHolder*)GetWidgetFromName(TEXT("Weather"));
	Weather->nCardHolder = CARD_LIST_LOC_WEATHERSLOT;
	Weather->nPlayer = PLAYER_INVALID;
	Weather->sTag = "Weather";

	ZoomHolder = (UCardsCardHolder*)GetWidgetFromName(TEXT("ZoomHolder"));
	ZoomHolder->nCardHolder = CARD_LIST_LOC_ZOOM;
	ZoomHolder->nPlayer = PLAYER_INVALID;
	ZoomHolder->sTag = "ZoomHolder";

	P1Hand = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1Hand"));
	P1Hand->nCardHolder = CARD_LIST_LOC_HAND;
	P1Hand->nPlayer = PLAYER_1;
	P1Hand->sTag = "P1Hand";
	P1Hand->SetVisibility(ESlateVisibility::Hidden);
	P1HandBackground = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1HandBackground"));
	P1HandBackground->SetVisibility(ESlateVisibility::Hidden);

	P2HandInvisible = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2HandInvisible"));
	P2HandInvisible->nCardHolder = CARD_LIST_LOC_HAND;
	P2HandInvisible->nPlayer = PLAYER_2;
	P2HandInvisible->sTag = "P2Hand";

	P1Siege = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1Siege"));
	P1Siege->nCardHolder = CARD_LIST_LOC_SIEGE;
	P1Siege->nPlayer = PLAYER_1;
	P1Siege->sTag = "P1Siege";

	P2Siege = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2Siege"));
	P2Siege->nCardHolder = CARD_LIST_LOC_SIEGE;
	P2Siege->nPlayer = PLAYER_2;
	P2Siege->sTag = "P2Siege";

	P1Ranged = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1Ranged"));
	P1Ranged->nCardHolder = CARD_LIST_LOC_RANGED;
	P1Ranged->nPlayer = PLAYER_1;
	P1Ranged->sTag = "P1Ranged";

	P2Ranged = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2Ranged"));
	P2Ranged->nCardHolder = CARD_LIST_LOC_RANGED;
	P2Ranged->nPlayer = PLAYER_2;
	P2Ranged->sTag = "P2Ranged";

	P1Melee = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1Melee"));
	P1Melee->nCardHolder = CARD_LIST_LOC_MELEE;
	P1Melee->nPlayer = PLAYER_1;
	P1Melee->sTag = "P1Melee";

	P2Melee = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2Melee"));
	P2Melee->nCardHolder = CARD_LIST_LOC_MELEE;
	P2Melee->nPlayer = PLAYER_2;
	P2Melee->sTag = "P2Melee";

	P1SiegeModifier = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1SiegeModifier"));
	P1SiegeModifier->nCardHolder = CARD_LIST_LOC_SIEGEMODIFIERS;
	P1SiegeModifier->nPlayer = PLAYER_1;
	P1SiegeModifier->sTag = "P1SiegeModifier";

	P2SiegeModifier = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2SiegeModifier"));
	P2SiegeModifier->nCardHolder = CARD_LIST_LOC_SIEGEMODIFIERS;
	P2SiegeModifier->nPlayer = PLAYER_2;
	P2SiegeModifier->sTag = "P2SiegeModifier";

	P1RangedModifier = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1RangedModifier"));
	P1RangedModifier->nCardHolder = CARD_LIST_LOC_RANGEDMODIFIERS;
	P1RangedModifier->nPlayer = PLAYER_1;
	P1RangedModifier->sTag = "P1RangedModifier";

	P2RangedModifier = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2RangedModifier"));
	P2RangedModifier->nCardHolder = CARD_LIST_LOC_RANGEDMODIFIERS;
	P2RangedModifier->nPlayer = PLAYER_2;
	P2RangedModifier->sTag = "P2RangedModifier";

	P1MeleeModifier = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1MeleeModifier"));
	P1MeleeModifier->nCardHolder = CARD_LIST_LOC_MELEEMODIFIERS;
	P1MeleeModifier->nPlayer = PLAYER_1;
	P1MeleeModifier->sTag = "P1MeleeModifier";

	P2MeleeModifier = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2MeleeModifier"));
	P2MeleeModifier->nCardHolder = CARD_LIST_LOC_MELEEMODIFIERS;
	P2MeleeModifier->nPlayer = PLAYER_2;
	P2MeleeModifier->sTag = "P2MeleeModifier";

	P1Graveyard = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1Graveyard"));
	P1Graveyard->nCardHolder = CARD_LIST_LOC_GRAVEYARD;
	P1Graveyard->nPlayer = PLAYER_1;
	P1Graveyard->sTag = "P1Graveyard";
	P1Graveyard->SetVisibility(ESlateVisibility::Hidden);

	P2Graveyard = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2Graveyard"));
	P2Graveyard->nCardHolder = CARD_LIST_LOC_GRAVEYARD;
	P2Graveyard->nPlayer = PLAYER_2;
	P2Graveyard->sTag = "P2Graveyard";
	P2Graveyard->SetVisibility(ESlateVisibility::Hidden);

	P1Deck = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1Deck"));
	P1Deck->nCardHolder = CARD_LIST_LOC_DECK;
	P1Deck->nPlayer = PLAYER_1;
	P1Deck->sTag = "P1Deck";

	P2Deck = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2Deck"));
	P2Deck->nCardHolder = CARD_LIST_LOC_DECK;
	P2Deck->nPlayer = PLAYER_2;
	P2Deck->sTag = "P2Deck";

	P1Leader = (UCardsCardHolder*)GetWidgetFromName(TEXT("P1Leader"));
	P1Leader->nCardHolder = CARD_LIST_LOC_LEADER;
	P1Leader->nPlayer = PLAYER_1;
	P1Leader->sTag = "P1Leader";

	P2Leader = (UCardsCardHolder*)GetWidgetFromName(TEXT("P2Leader"));
	P2Leader->nCardHolder = CARD_LIST_LOC_LEADER;
	P2Leader->nPlayer = PLAYER_2;
	P2Leader->sTag = "P2Leader";

	P1LeaderUse = (UBorder*)GetWidgetFromName(TEXT("P1LeaderUse"));
	P2LeaderUse = (UBorder*)GetWidgetFromName(TEXT("P2LeaderUse"));
	P1LeaderDisable = (UBorder*)GetWidgetFromName(TEXT("P1LeaderDisable"));
	P2LeaderDisable = (UBorder*)GetWidgetFromName(TEXT("P2LeaderDisable"));
	P1LeaderUse->SetVisibility(ESlateVisibility::Hidden);
	P2LeaderUse->SetVisibility(ESlateVisibility::Hidden);
	P1LeaderDisable->SetVisibility(ESlateVisibility::Hidden);
	P2LeaderDisable->SetVisibility(ESlateVisibility::Hidden);

	P1Life0 = (UBorder*)GetWidgetFromName(TEXT("P1Life0"));
	P1Life1 = (UBorder*)GetWidgetFromName(TEXT("P1Life1"));
	P2Life0 = (UBorder*)GetWidgetFromName(TEXT("P2Life0"));
	P2Life1 = (UBorder*)GetWidgetFromName(TEXT("P2Life1"));
	P1Life0->SetRenderOpacity(0.33f);
	P1Life1->SetRenderOpacity(0.33f);
	P2Life0->SetRenderOpacity(0.33f);
	P2Life1->SetRenderOpacity(0.33f);

	P1DeckBorder = (UBorder*)GetWidgetFromName(TEXT("P1DeckBorder"));
	P2DeckBorder = (UBorder*)GetWidgetFromName(TEXT("P2DeckBorder"));

	P1DeckText = (UTextBlock*)GetWidgetFromName(TEXT("P1DeckText"));
	P2DeckText = (UTextBlock*)GetWidgetFromName(TEXT("P2DeckText"));

	P1CardsHandText = (UTextBlock*)GetWidgetFromName(TEXT("P1CardsHandText"));
	P2CardsHandText = (UTextBlock*)GetWidgetFromName(TEXT("P2CardsHandText"));

	P1ScoreAllText = (UTextBlock*)GetWidgetFromName(TEXT("P1ScoreAllText"));
	P2ScoreAllText = (UTextBlock*)GetWidgetFromName(TEXT("P2ScoreAllText"));
	P2ScoreSiegeText = (UTextBlock*)GetWidgetFromName(TEXT("P2ScoreSiegeText"));
	P2ScoreRangedText = (UTextBlock*)GetWidgetFromName(TEXT("P2ScoreRangedText"));
	P2ScoreMeleeText = (UTextBlock*)GetWidgetFromName(TEXT("P2ScoreMeleeText"));
	P1ScoreMeleeText = (UTextBlock*)GetWidgetFromName(TEXT("P1ScoreMeleeText"));
	P1ScoreRangedText = (UTextBlock*)GetWidgetFromName(TEXT("P1ScoreRangedText"));
	P1ScoreSiegeText = (UTextBlock*)GetWidgetFromName(TEXT("P1ScoreSiegeText"));

	P1PassedText = (UTextBlock*)GetWidgetFromName(TEXT("P1PassedText"));
	P2PassedText = (UTextBlock*)GetWidgetFromName(TEXT("P2PassedText"));
	P1PassedText->SetVisibility(ESlateVisibility::Hidden);
	P2PassedText->SetVisibility(ESlateVisibility::Hidden);

	P1FactionText = (UTextBlock*)GetWidgetFromName(TEXT("P1FactionText"));
	P2FactionText = (UTextBlock*)GetWidgetFromName(TEXT("P2FactionText"));

	BorderAccept = (UBorder*)GetWidgetFromName(TEXT("BorderAccept"));
	BorderCancel = (UBorder*)GetWidgetFromName(TEXT("BorderCancel"));
	TextAccept = (UTextBlock*)GetWidgetFromName(TEXT("TextAccept"));
	TextCancel = (UTextBlock*)GetWidgetFromName(TEXT("TextCancel"));
	BorderAccept->SetVisibility(ESlateVisibility::Hidden);
	BorderCancel->SetVisibility(ESlateVisibility::Hidden);
	TextAccept->SetVisibility(ESlateVisibility::Hidden);
	TextCancel->SetVisibility(ESlateVisibility::Hidden);
	FName BinderFunctionAccept(TEXT("TransactionAccept"));
	FName BinderFunctionCancel(TEXT("TransactionCancel"));
	BorderAccept->OnMouseButtonUpEvent.BindUFunction(this, BinderFunctionAccept);
	BorderCancel->OnMouseButtonUpEvent.BindUFunction(this, BinderFunctionCancel);

	BorderSkipTurn = (UBorder*)GetWidgetFromName(TEXT("BorderSkipTurn"));
	TextSkipTurn = (UTextBlock*)GetWidgetFromName(TEXT("TextSkipTurn"));
	BorderSkipTurn->SetVisibility(ESlateVisibility::Hidden);
	TextSkipTurn->SetVisibility(ESlateVisibility::Hidden);
	FName BinderFunctionSkipTurn(TEXT("HandleSkipTurn"));
	BorderSkipTurn->OnMouseButtonUpEvent.BindUFunction(this, BinderFunctionSkipTurn);

	BorderShowHand = (UBorder*)GetWidgetFromName(TEXT("BorderShowHand"));
	TextShowHand = (UTextBlock*)GetWidgetFromName(TEXT("TextShowHand"));
	BorderShowHand->SetVisibility(ESlateVisibility::Visible);
	TextShowHand->SetVisibility(ESlateVisibility::HitTestInvisible);
	TextShowHand->SetAutoWrapText(true);
	FName BinderFunctionShowHand(TEXT("HandleShowHand"));
	BorderShowHand->OnMouseButtonUpEvent.BindUFunction(this, BinderFunctionShowHand);

	BorderStateBackground = (UBorder*)GetWidgetFromName(TEXT("BorderStateBackground"));
	BorderState = (UBorder*)GetWidgetFromName(TEXT("BorderState"));
	TextState = (UTextBlock*)GetWidgetFromName(TEXT("TextChangeState"));
	BorderStateBackground->SetVisibility(ESlateVisibility::Hidden);
	BorderState->SetVisibility(ESlateVisibility::Hidden);
	TextState->SetVisibility(ESlateVisibility::Hidden);

	FrameRow = (UImage*)GetWidgetFromName(TEXT("FrameRow"));
	FrameRow->SetVisibility(ESlateVisibility::Hidden);

	FrameWeather = (UImage*)GetWidgetFromName(TEXT("FrameWeather"));
	FrameWeather->SetVisibility(ESlateVisibility::Hidden);
}

void UCardsGameMain::SpawnCardInstance(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer)
{
	UCardSlot* cardSlot = nullptr;

	if (nCardListLocation == CARD_LIST_LOC_ZOOM)
	{
		if (GetCurrentPlayerController()->wCardSlotZoom)
		{
			cardSlot = CreateWidget<UCardSlot>(GetCurrentPlayerController(), GetCurrentPlayerController()->wCardSlotZoom);
			transactionCardSlot = cardSlot;
		}
	}
	else
	{
		if (GetCurrentPlayerController()->wCardSlotBoard)
		{
			cardSlot = CreateWidget<UCardSlot>(GetCurrentPlayerController(), GetCurrentPlayerController()->wCardSlotBoard);
		}
	}

	cardSlot->cardInstanceRef = cardInstance;
	cardSlot->nInstanceId = cardInstance->nInstanceId;
	cardSlot->nCardIndex = cardInstance->templateRef->nIndex;

	HandleCardSlot(*cardSlot, nCardListLocation, nPlayer);

	WasAddedToList(cardInstance, nCardListLocation, nPlayer);
}

void UCardsGameMain::WasAddedToList(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer)
{
	UCardsCardHolder* holder = GetCardHolder(nCardListLocation, nPlayer);
	UCardSlot* cardSlot = GetCardSlotFromCardInstance(cardInstance);

	if (!holder || !cardSlot)
	{
		LogError("GFX ---- spawnCardInstance failed because it was called with unknown params, sourceTypeID: "
			+ FString::FromInt(nCardListLocation) + ", sourcePlayerID: " + FString::FromInt(nPlayer));
	}

	holder->CardAdded(cardSlot);
}

void UCardsGameMain::HandleCardSlot(UCardSlot& cardSlot, int32 nCardListLocation, int32 nPlayer)
{
	FString sLocation;
	switch (nCardListLocation)
	{
	case CARD_LIST_LOC_ZOOM:
	{
		ZoomHolder->AddChildToCanvas(&cardSlot);
		RearrangeSlots(ZoomHolder);

		if (transactionCardSlot && transactionCardSlot->CardDescription)
			transactionCardSlot->CardDescription->SetVisibility(ESlateVisibility::HitTestInvisible);

		/*if (transactionCardSlot && transactionCardSlot->CardCounter)
			transactionCardSlot->CardCounter->SetVisibility(ESlateVisibility::HitTestInvisible);*/

		GetBoardRenderer()->HandleEffectDescription(false);

		break;
	}
	case CARD_LIST_LOC_WEATHERSLOT:
	{
		Weather->AddChildToCanvas(&cardSlot);
		RearrangeSlots(Weather);
		break;
	}
	case CARD_LIST_LOC_LEADER:
	{
		if (nPlayer == PLAYER_1)
		{
			P1Leader->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1Leader);
		}
		else if (nPlayer == PLAYER_2)
		{
			P2Leader->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P2Leader);
		}

		break;
	}
	case CARD_LIST_LOC_HAND:
	{
		if (nPlayer == PLAYER_1)
		{
			P1Hand->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1Hand);
		}
		else if (nPlayer == PLAYER_2)
			P2HandInvisible->AddChildToCanvas(&cardSlot);

		break;
	}
	case CARD_LIST_LOC_MELEE:
	{
		GetBoardRenderer()->PlaySound("Melee");

		if (nPlayer == PLAYER_1)
		{
			P1Melee->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1Melee);
		}
		else if (nPlayer == PLAYER_2)
		{
			P2Melee->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P2Melee);
		}

		break;
	}
	case CARD_LIST_LOC_RANGED:
	{
		GetBoardRenderer()->PlaySound("Ranged");

		if (nPlayer == PLAYER_1)
		{
			P1Ranged->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1Ranged);
		}
		else if (nPlayer == PLAYER_2)
		{
			P2Ranged->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P2Ranged);
		}

		break;
	}
	case CARD_LIST_LOC_SIEGE:
	{
		GetBoardRenderer()->PlaySound("Siege");

		if (nPlayer == PLAYER_1)
		{
			P1Siege->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1Siege);
		}
		else if (nPlayer == PLAYER_2)
		{
			P2Siege->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P2Siege);
		}

		break;
	}
	case CARD_LIST_LOC_MELEEMODIFIERS:
	{
		if (nPlayer == PLAYER_1)
		{
			P1MeleeModifier->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1MeleeModifier);
		}
		else if (nPlayer == PLAYER_2)
		{
			P2MeleeModifier->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P2MeleeModifier);
		}

		break;
	}
	case CARD_LIST_LOC_RANGEDMODIFIERS:
	{
		if (nPlayer == PLAYER_1)
		{
			P1RangedModifier->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1RangedModifier);
		}
		else if (nPlayer == PLAYER_2)
		{
			P2RangedModifier->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P2RangedModifier);
		}

		break;
	}
	case CARD_LIST_LOC_SIEGEMODIFIERS:
	{
		if (nPlayer == PLAYER_1)
		{
			P1SiegeModifier->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1SiegeModifier);
		}
		else if (nPlayer == PLAYER_2)
		{
			P2SiegeModifier->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P2SiegeModifier);
		}

		break;
	}
	case CARD_LIST_LOC_GRAVEYARD:
	{
		if (nPlayer == PLAYER_1)
		{
			P1Graveyard->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P1Graveyard);
		}
		else if (nPlayer == PLAYER_2)
		{
			P2Graveyard->AddChildToCanvas(&cardSlot);
			RearrangeSlots(P2Graveyard);
		}

		break;
	}
	default:
		break;
	}

	//text power
	if (nCardListLocation != CARD_LIST_LOC_LEADER)
	{
		if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Creature))
		{
			int32 nColor = 0;

			if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Hero))
				nColor = 16777215;

			float r = ((nColor >> 16) & 255) / 255;
			float g = ((nColor >> 8) & 255) / 255;
			float b = (nColor & 255) / 255;
			float a = 255 / 255;

			cardSlot.TextPower->SetColorAndOpacity(FLinearColor(r, g, b, a));
			cardSlot.TextPower->SetText(FText::FromString(FString::FromInt(cardSlot.cardInstanceRef->templateRef->nPower)));
		}
		else
		{
			if (cardSlot.TextPower)
				cardSlot.TextPower->SetText(FText::FromString(""));
		}
	}
	else
	{
		if (cardSlot.TextPower)
			cardSlot.TextPower->SetText(FText::FromString(""));
	}

	//banner
	if (cardSlot.BorderFaction)
	{
		int32 nBannerIcon = 0;
		if (cardSlot.cardInstanceRef->templateRef->nFactionIdx > FactionId_Neutral)
		{
			nBannerIcon = cardSlot.cardInstanceRef->templateRef->nFactionIdx;
			UTexture2D* textureBanner = *GetCurrentPlayerController()->factionBannerTextures.Find(nBannerIcon);
			FSlateBrush brushBanner = UWidgetBlueprintLibrary::MakeBrushFromTexture(textureBanner);
			cardSlot.BorderFaction->SetBrush(brushBanner);
		}
	}

	//power icon
	if (nCardListLocation != CARD_LIST_LOC_LEADER
		&& !cardSlot.cardInstanceRef->IsA(UCardLeaderInstance::StaticClass()))
	{
		int32 nPowerIcon = 0;
		if (cardSlot.cardInstanceRef->templateRef->nPower > 0)
		{
			if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Creature))
			{
				nPowerIcon = CardType_Creature;
			}
			if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Hero))
			{
				nPowerIcon = CardType_Hero;
			}

			UTexture2D* texturePower = *GetCurrentPlayerController()->typeTextures.Find(nPowerIcon);
			FSlateBrush brushPower = UWidgetBlueprintLibrary::MakeBrushFromTexture(texturePower);

			//TODO if hero card, or other non creature?
			if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Hero))
				cardSlot.BorderPowerHero->SetBrush(brushPower);
			else cardSlot.BorderPower->SetBrush(brushPower);
		}
		else
		{
			if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Weather))
			{
				if (cardSlot.cardInstanceRef->templateRef->HasEffect(CardEffect_Melee))
					nPowerIcon = CardEffect_Melee;
				else if (cardSlot.cardInstanceRef->templateRef->HasEffect(CardEffect_Ranged))
					nPowerIcon = CardEffect_Ranged;
				else if (cardSlot.cardInstanceRef->templateRef->HasEffect(CardEffect_Siege))
					nPowerIcon = CardEffect_Siege;
				else if (cardSlot.cardInstanceRef->templateRef->HasEffect(CardEffect_ClearSky))
					nPowerIcon = CardEffect_ClearSky;
			}
			else LogError("Unknown power card");

			UTexture2D* texturePower = *GetCurrentPlayerController()->effectTextures.Find(nPowerIcon);
			FSlateBrush brushPower = UWidgetBlueprintLibrary::MakeBrushFromTexture(texturePower);
			cardSlot.BorderPower->SetBrush(brushPower);
		}
	}

	//type icon
	if (nCardListLocation != CARD_LIST_LOC_LEADER)
	{
		int32 nTypeIcon = 0;
		if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Creature))
		{
			if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Melee))
			{
				nTypeIcon = CardType_Melee;
			}
			else if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Ranged))
			{
				nTypeIcon = CardType_Ranged;
			}
			else if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_RangedMelee))
			{
				nTypeIcon = CardType_RangedMelee;
			}
			else if (cardSlot.cardInstanceRef->templateRef->IsType(CardType_Siege))
			{
				nTypeIcon = CardType_Siege;
			}

			UTexture2D* textureType = *GetCurrentPlayerController()->typeTextures.Find(nTypeIcon);
			FSlateBrush brushType = UWidgetBlueprintLibrary::MakeBrushFromTexture(textureType);
			cardSlot.BorderType->SetBrush(brushType);
		}
	}

	//effect icon
	if (nCardListLocation != CARD_LIST_LOC_LEADER)
	{
		if (!cardSlot.cardInstanceRef->templateRef->HasEffect(CardEffect_None)
			&& cardSlot.cardInstanceRef->templateRef->IsType(CardType_Creature))
		{
			UTexture2D* textureEffect = *GetCurrentPlayerController()->effectTextures.Find(cardSlot.cardInstanceRef->templateRef->effectFlags[0]);
			FSlateBrush brushEffect = UWidgetBlueprintLibrary::MakeBrushFromTexture(textureEffect);
			cardSlot.BorderEffect->SetBrush(brushEffect);
		}
	}

	//Title text
	if (cardSlot.CardDescription && cardSlot.CardDescription->TextCardTitle)
		cardSlot.CardDescription->TextCardTitle->SetText(FText::FromString(cardSlot.cardInstanceRef->templateRef->sTitle));

	//Description text
	if (cardSlot.CardDescription && cardSlot.CardDescription->TextCardDescription)
		cardSlot.CardDescription->TextCardDescription->SetText(FText::FromString(cardSlot.cardInstanceRef->templateRef->sDescription));

	//picture
	int32 nPicIcon = cardSlot.cardInstanceRef->templateRef->nIndex;
	UTexture2D* texturePic = *GetCurrentPlayerController()->picTextures.Find(nPicIcon);
	FSlateBrush brushPic = UWidgetBlueprintLibrary::MakeBrushFromTexture(texturePic);
	cardSlot.BorderPicture->SetBrush(brushPic);
}

void UCardsGameMain::RearrangeSlots(UCardsCardHolder* cardHolder)
{
	int32 nRelativeX = 0;
	int32 nNewX = 0;
	int32 nDeltaX = 0;
	int32 nY = 0;

	UCanvasPanelSlot* canvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(cardHolder);

	FVector2D _pos = canvasPanelSlot->GetPosition();
	FVector2D _size = canvasPanelSlot->GetSize();

	if (cardHolder->GetChildrenCount() > 0)
	{
		nRelativeX = (cardHolder->GetChildrenCount() - 1) * PADDING_X + cardHolder->GetChildrenCount() * CARD_BOARD_WIDTH;
		nNewX = _size.X / 2 - nRelativeX / 2;
		nDeltaX = CARD_BOARD_WIDTH + PADDING_X;

		if (nRelativeX > _size.X)
		{
			nDeltaX = nDeltaX - (nRelativeX - _size.X) / (cardHolder->GetChildrenCount() - 1);
		}

		for (int32 i = 0; i < cardHolder->GetChildrenCount(); i++)
		{
			UWidget* cardWidget = cardHolder->GetChildAt(i);
			UCanvasPanelSlot* cardWidgetSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(cardWidget);
			if (cardHolder == ZoomHolder)
			{
				cardWidgetSlot->SetSize(FVector2D(CARD_ORIGIN_WIDTH, CARD_ORIGIN_HEIGHT));
			}
			else
			{
				cardWidgetSlot->SetSize(FVector2D(CARD_BOARD_WIDTH, CARD_BOARD_HEIGHT));

				nY = _size.Y - cardWidgetSlot->GetSize().Y;

				if (i > 0)
				{
					UWidget* cardPreviousWidget = cardHolder->GetChildAt(i - 1);
					UCanvasPanelSlot* cardPreviousWidgetSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(cardPreviousWidget);

					cardWidgetSlot->SetPosition(FVector2D(cardPreviousWidgetSlot->GetPosition().X + nDeltaX, nY));
				}
				else
				{
					if (nNewX < 0) nNewX = PADDING_X;
					cardWidgetSlot->SetPosition(FVector2D(nNewX, nY));
				}
			}


			LogWarning("Card " + FString::FromInt(i) + " in pos " + cardWidgetSlot->GetPosition().ToString());


		}
	}

	cardHolder = nullptr;
}

void UCardsGameMain::TransactionAccept()
{
	if (!transactionCardSlot->cardInstanceRef->IsA(UCardLeaderInstance::StaticClass()))
	{
		HandleShowHand();
	}

	(Cast<UHumanPlayerController>(GetGameFlowController()->playerControllers[PLAYER_1]))->HandleHolderChosen(destinationCardHolder);
}

void UCardsGameMain::TransactionCancel()
{
	ActivateHoldersForCard(nullptr, true);

	UCardSlot* _cardZoom = Cast<UCardSlot>(ZoomHolder->GetChildAt(0));
	UCardSlot* _card = GetCardSlotFromCardInstance(_cardZoom->cardInstanceRef);

	if (_card)
	{
		if (_card->GetVisibility() == ESlateVisibility::Hidden)
		{
			_card->SetVisibility(ESlateVisibility::Visible);

			GetGameFlowController()->playerControllers[PLAYER_1]->GetFiniteStateMachine()->ChangeState(STATE_CHOOSINGMOVE);

			ClearCardTransactionZoom();
		}
	}
}

void UCardsGameMain::HandleSkipTurn()
{
	Cast<UHumanPlayerController>(GetGameFlowController()->playerControllers[PLAYER_1])->SkipTurn();
}

void UCardsGameMain::HandleShowHand()
{
	bShowHand = !bShowHand;

	if (!bShowHand)
	{
		P1Hand->SetVisibility(ESlateVisibility::Hidden);
		P1HandBackground->SetVisibility(ESlateVisibility::Hidden);
		TextShowHand->SetText(FText::FromString("Show Hand"));
	}
	else
	{
		P1Hand->SetVisibility(ESlateVisibility::Visible);
		P1HandBackground->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextShowHand->SetText(FText::FromString("Hide Hand"));
	}
}

void UCardsGameMain::UpdateDeckCounter()
{
	P1DeckText->SetText(FText::FromString(FString::FromInt(GetCardManager()->playerDeckDefinitions[PLAYER_1]->cardIndicesInDeck.Num())));
	P2DeckText->SetText(FText::FromString(FString::FromInt(GetCardManager()->playerDeckDefinitions[PLAYER_2]->cardIndicesInDeck.Num())));
}

void UCardsGameMain::UpdateScore(int32 nP1Score, int32 nP2Score)
{
	P1ScoreAllText->SetText(FText::FromString(FString::FromInt(nP1Score)));
	P2ScoreAllText->SetText(FText::FromString(FString::FromInt(nP2Score)));
}

void UCardsGameMain::UpdateRowScore(int32 nP1Siege, int32 nP1Ranged, int32 nP1Melee, int32 nP2Melee, int32 nP2Ranged, int32 nP2Siege)
{
	P1ScoreSiegeText->SetText(FText::FromString(FString::FromInt(nP1Siege)));
	P1ScoreRangedText->SetText(FText::FromString(FString::FromInt(nP1Ranged)));
	P1ScoreMeleeText->SetText(FText::FromString(FString::FromInt(nP1Melee)));
	P2ScoreMeleeText->SetText(FText::FromString(FString::FromInt(nP2Melee)));
	P2ScoreRangedText->SetText(FText::FromString(FString::FromInt(nP2Ranged)));
	P2ScoreSiegeText->SetText(FText::FromString(FString::FromInt(nP2Siege)));

}

void UCardsGameMain::UpdateCardsInHand(int32 nPlayer, int32 nCardsCount)
{
	if (nPlayer == PLAYER_1)
		P1CardsHandText->SetText(FText::FromString(FString::FromInt(nCardsCount)));
	else if (nPlayer == PLAYER_2)
		P2CardsHandText->SetText(FText::FromString(FString::FromInt(nCardsCount)));
}

void UCardsGameMain::UpdateFactionText()
{
	P1FactionText->SetText(FText::FromString(ConvertFactionToString((GetCardManager()->playerDeckDefinitions[PLAYER_1]->GetDeckFaction()))));
	P2FactionText->SetText(FText::FromString(ConvertFactionToString((GetCardManager()->playerDeckDefinitions[PLAYER_2]->GetDeckFaction()))));
}

void UCardsGameMain::UpdateFactionDeckCover()
{
	int32 nP1Faction = (GetCardManager()->playerDeckDefinitions[PLAYER_1]->GetDeckFaction());
	UTexture2D* textureP1DeckCover = *GetCurrentPlayerController()->factionDeckTextures.Find(nP1Faction);
	FSlateBrush brushP1DeckCover = UWidgetBlueprintLibrary::MakeBrushFromTexture(textureP1DeckCover);
	P1DeckBorder->SetBrush(brushP1DeckCover);

	int32 nP2Faction = (GetCardManager()->playerDeckDefinitions[PLAYER_2]->GetDeckFaction());
	UTexture2D* textureP2DeckCover = *GetCurrentPlayerController()->factionDeckTextures.Find(nP2Faction);
	FSlateBrush brushP2DeckCover = UWidgetBlueprintLibrary::MakeBrushFromTexture(textureP2DeckCover);
	P2DeckBorder->SetBrush(brushP2DeckCover);
}

void UCardsGameMain::HandleTransactionButtons(bool bHide, UCardInstance* cardInstance /*= nullptr*/)
{
	if (bHide)
	{
		BorderAccept->SetVisibility(ESlateVisibility::Hidden);
		BorderCancel->SetVisibility(ESlateVisibility::Hidden);
		TextAccept->SetVisibility(ESlateVisibility::Hidden);
		TextCancel->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		BorderCancel->SetVisibility(ESlateVisibility::Visible);
		TextCancel->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (cardInstance && cardInstance->nOwningPlayer == PLAYER_1)//only for human player
		{
			if (cardInstance->nLastListApplied == CARD_LIST_LOC_HAND || // only for hand 
				(cardInstance->nLastListApplied == CARD_LIST_LOC_LEADER &&
					P1LeaderDisable->GetVisibility() == ESlateVisibility::Hidden))// and unused leader
			{
				BorderAccept->SetVisibility(ESlateVisibility::Visible);
				TextAccept->SetVisibility(ESlateVisibility::HitTestInvisible);
			}
		}
	}
}

void UCardsGameMain::HandleEffectDescription(bool bHide)
{
	if (bHide)
	{
		transactionCardSlot->CardDescription->EffectBackground->SetVisibility(ESlateVisibility::Hidden);
		transactionCardSlot->CardDescription->EffectTypeBorder->SetVisibility(ESlateVisibility::Hidden);
		transactionCardSlot->CardDescription->EffectTitle->SetVisibility(ESlateVisibility::Hidden);
		transactionCardSlot->CardDescription->EffectDescription->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		if (transactionCardSlot->cardInstanceRef->templateRef->HasEffect(CardEffect_None))
		{
			transactionCardSlot->CardDescription->EffectBackground->SetVisibility(ESlateVisibility::Hidden);
			transactionCardSlot->CardDescription->EffectTypeBorder->SetVisibility(ESlateVisibility::Hidden);
			transactionCardSlot->CardDescription->EffectTitle->SetVisibility(ESlateVisibility::Hidden);
			transactionCardSlot->CardDescription->EffectDescription->SetVisibility(ESlateVisibility::Hidden);

			return;
		}
		else
		{
			//leaders don't have effect icons
			if (!transactionCardSlot->cardInstanceRef->IsA(UCardLeaderInstance::StaticClass()))
			{
				UTexture2D* textureEffect = *GetCurrentPlayerController()->effectTextures.Find(transactionCardSlot->cardInstanceRef->templateRef->effectFlags[0]);
				FSlateBrush brushEffect = UWidgetBlueprintLibrary::MakeBrushFromTexture(textureEffect);
				transactionCardSlot->CardDescription->EffectTypeBorder->SetBrush(brushEffect);
			}

			FString sTitle;
			sTitle = transactionCardSlot->cardInstanceRef->templateRef->GetEffectString();
			if (sTitle == "None") sTitle = transactionCardSlot->cardInstanceRef->templateRef->sTitle;

			transactionCardSlot->CardDescription->EffectTitle->SetText(FText::FromString(sTitle));

			transactionCardSlot->CardDescription->EffectDescription->SetText(FText::FromString(transactionCardSlot->cardInstanceRef->templateRef->GetTooltipString()));

			transactionCardSlot->CardDescription->EffectBackground->SetVisibility(ESlateVisibility::HitTestInvisible);
			transactionCardSlot->CardDescription->EffectTypeBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
			transactionCardSlot->CardDescription->EffectTitle->SetVisibility(ESlateVisibility::HitTestInvisible);
			transactionCardSlot->CardDescription->EffectDescription->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UCardsGameMain::HandleSkipTurnButton(bool bHide)
{
	if (bHide)
	{
		BorderSkipTurn->SetVisibility(ESlateVisibility::Hidden);
		TextSkipTurn->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		BorderSkipTurn->SetVisibility(ESlateVisibility::Visible);
		TextSkipTurn->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UCardsGameMain::ClearCardTransactionZoom()
{
	HandleTransactionButtons(true);
	ZoomHolder->RemoveChildAt(0);
	transactionCardSlot = nullptr;
}

void UCardsGameMain::HandleChangeStates(int32 nState, int32 nPlayer, bool bHide)
{
	if (bHide)
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::Hidden);
		TextState->SetVisibility(ESlateVisibility::Hidden);
		BorderState->SetVisibility(ESlateVisibility::Hidden);

		return;
	}

	switch (nState)
	{
	case STATE_COINTOSS:
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::HitTestInvisible);

		TextState->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (nPlayer == PLAYER_1)
		{
			nState = STATE_COINTOSS_PLAYER;
			TextState->SetText(FText::FromString("You will go first"));
		}
		else if (nPlayer == PLAYER_2)
		{
			nState = STATE_COINTOSS_OPPONENT;
			TextState->SetText(FText::FromString("Your opponent will go first"));
		}

		break;
	}
	case STATE_ROUNDSTART:
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::HitTestInvisible);

		TextState->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextState->SetText(FText::FromString("Round Start"));

		P1PassedText->SetVisibility(ESlateVisibility::Hidden);
		P2PassedText->SetVisibility(ESlateVisibility::Hidden);

		break;
	}
	case STATE_ROUNDPASSED:
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::HitTestInvisible);

		TextState->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextState->SetText(FText::FromString("Round Passed"));

		if (nPlayer == PLAYER_1)
			P1PassedText->SetVisibility(ESlateVisibility::HitTestInvisible);
		else if (nPlayer == PLAYER_2)
			P2PassedText->SetVisibility(ESlateVisibility::HitTestInvisible);

		break;
	}
	case STATE_ROUNDLOST:
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::HitTestInvisible);

		TextState->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextState->SetText(FText::FromString("Your opponent wins the round."));

		break;
	}
	case STATE_ROUNDWON:
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::HitTestInvisible);

		TextState->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextState->SetText(FText::FromString("You won the round!"));

		break;
	}
	case STATE_PLAYERTURN:
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextState->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (nPlayer == PLAYER_1)
		{
			nState = STATE_PLAYERTURN_PLAYER;
			TextState->SetText(FText::FromString("Your turn!"));
		}
		else if (nPlayer == PLAYER_2)
		{
			nState = STATE_PLAYERTURN_OPPONENT;
			TextState->SetText(FText::FromString("Opponent's turn"));
		}

		break;
	}
	case STATE_ABILITY_NILFGAARD:
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::HitTestInvisible);

		TextState->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextState->SetText(FText::FromString("Paragon 2 faction ability triggered - Paragon 2 wins the tie."));

		break;
	}
	case STATE_ABILITY_NORTHERN:
	{
		BorderStateBackground->SetVisibility(ESlateVisibility::HitTestInvisible);

		TextState->SetVisibility(ESlateVisibility::HitTestInvisible);
		TextState->SetText(FText::FromString("Paragon 1 faction ability triggered - Paragon 1 draws an additional card."));

		break;
	}
	default:
	{
		FString sString = "Unknown state " + FString::FromInt(nState);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);
		break;
	}
	}

	BorderState->SetVisibility(ESlateVisibility::HitTestInvisible);
	UTexture2D* textureState = *GetCurrentPlayerController()->statesTextures.Find(nState);
	FSlateBrush brushState = UWidgetBlueprintLibrary::MakeBrushFromTexture(textureState);
	BorderState->SetBrush(brushState);
}

void UCardsGameMain::ActivateHoldersForCard(UCardInstance* cardInstance, bool bHide)
{
	if (bHide)
	{
		FrameRow->SetVisibility(ESlateVisibility::Hidden);
		FrameWeather->SetVisibility(ESlateVisibility::Hidden);
		if (transactionCardSlot && transactionCardSlot->CardDescription)
			transactionCardSlot->CardDescription->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		bool bHolder = false;

		UCanvasPanel* canvas = Cast<UCanvasPanel>(this->WidgetTree->RootWidget);

		for (int32 i = 0; i < canvas->GetChildrenCount(); i++)
		{
			UWidget* _childCanvas = canvas->GetChildAt(i);
			if (_childCanvas && _childCanvas->IsA(UCardsCardHolder::StaticClass()))
			{
				UCardsCardHolder* _holder = Cast<UCardsCardHolder>(_childCanvas);
				bHolder = false;

				if (cardInstance->templateRef->HasEffect(CardEffect_UnsummonDummy)
					&& _holder->nPlayer == cardInstance->nOwningPlayer
					&& (_holder->nCardHolder == CARD_LIST_LOC_MELEE
						|| _holder->nCardHolder == CARD_LIST_LOC_RANGED
						|| _holder->nCardHolder == CARD_LIST_LOC_SIEGE)
					&& _holder->nPlayer == cardInstance->nOwningPlayer)
				{
					TArray<UCardInstance*> creaturesList;
					GetCardManager()->GetAllCreaturesNonHero(_holder->nCardHolder, _holder->nPlayer, creaturesList);
					bHolder = creaturesList.Num() > 0;
				}
				else
				{
					if (cardInstance->CanBePlacedInSlot(_holder->nCardHolder, _holder->nPlayer))
					{
						bHolder = true;
					}
				}

				FString sValid = bHolder == true ? "True" : "False";
				LogWarning("GFX ----- Analyzing slot for placement, valid: " + sValid + ", for slot: " + _holder->sTag);

				if (bHolder)
				{
					destinationCardHolder = _holder;

					UCanvasPanelSlot* slotHolder = UWidgetLayoutLibrary::SlotAsCanvasSlot(_holder);

					if (cardInstance->templateRef->IsType(CardType_Weather))
					{
						FrameWeather->SetVisibility(ESlateVisibility::HitTestInvisible);
						FrameWeather->SetRenderTranslation(slotHolder->GetPosition());
					}
					else
					{
						FrameRow->SetVisibility(ESlateVisibility::HitTestInvisible);
						FrameRow->SetRenderTranslation(slotHolder->GetPosition());
					}
				}
			}
		}
	}
}

void UCardsGameMain::RemoveCardFromHolder(UCardInstance* cardInstance)
{
	if (GetCardSlotFromCardInstance(cardInstance))
		GetCardSlotFromCardInstance(cardInstance)->RemoveFromParent();
	RearrangeSlots(destinationCardHolder);

}

void UCardsGameMain::PlaySound(FString sSound)
{
	USoundCue* SoundCue = GetCurrentPlayerController()->sounds[sSound];
	if (SoundCue)
		UGameplayStatics::PlaySound2D(GetCurrentWorld(), SoundCue);
}

UCardSlot* UCardsGameMain::GetCardSlotFromCardInstance(UCardInstance* cardInstance)
{
	UCanvasPanel* canvas = Cast<UCanvasPanel>(this->WidgetTree->RootWidget);

	for (int32 i = 0; i < canvas->GetChildrenCount(); i++)
	{
		UWidget* _childHolder = canvas->GetChildAt(i);
		if (_childHolder && _childHolder->IsA(UCardsCardHolder::StaticClass()))
		{
			UCardsCardHolder* _holder = Cast<UCardsCardHolder>(_childHolder);

			for (int32 j = 0; j < _holder->GetChildrenCount(); j++)
			{
				UWidget* _childSlot = _holder->GetChildAt(j);

				if (_childSlot && _childSlot->IsA(UCardSlot::StaticClass()))
				{
					UCardSlot* _card = Cast<UCardSlot>(_childSlot);

					if (_card->cardInstanceRef == cardInstance)
					{
						destinationCardHolder = _holder;
						return _card;
					}
				}
			}
		}
	}

	return nullptr;
}

UCardsCardHolder* UCardsGameMain::GetCardHolder(int32 nCardListLocation, int32 nPlayer)
{
	UCanvasPanel* canvas = Cast<UCanvasPanel>(this->WidgetTree->RootWidget);

	for (int32 i = 0; i < canvas->GetChildrenCount(); i++)
	{
		UWidget* _childCanvas = canvas->GetChildAt(i);
		if (_childCanvas && _childCanvas->IsA(UCardsCardHolder::StaticClass()))
		{
			UCardsCardHolder* _holder = Cast<UCardsCardHolder>(_childCanvas);

			//common
			if (nCardListLocation == CARD_LIST_LOC_ZOOM
				|| nCardListLocation == CARD_LIST_LOC_WEATHERSLOT)
			{
				return _holder;
			}
			else if (_holder->nCardHolder == nCardListLocation
				&& _holder->nPlayer == nPlayer)
			{
				return _holder;
			}
		}
	}

	return nullptr;
}

void UCardsGameMain::DisableLeaderUse(int32 nPlayer)
{
	switch (nPlayer)
	{
	case PLAYER_1:
	{
		P1LeaderDisable->SetVisibility(ESlateVisibility::HitTestInvisible);
		P1LeaderUse->SetVisibility(ESlateVisibility::Hidden);

		break;
	}
	case PLAYER_2:
	{
		P2LeaderDisable->SetVisibility(ESlateVisibility::HitTestInvisible);
		P2LeaderUse->SetVisibility(ESlateVisibility::Hidden);

		break;
	}
	default:
		break;
	}
}

void UCardsGameMain::SetPlayerLives(int32 nPlayer, int32 nLives)
{
	//DHK reversed now, as we display half a Winner Cup when winning
	LogWarning("GFX - Updating life for Player: " + FString::FromInt(nPlayer) + ", to: " + FString::FromInt(nLives));
	switch (nLives)
	{
	case 0:
	{
		if (nPlayer == PLAYER_1)
			P2Life0->SetRenderOpacity(1.f);
		else if (nPlayer == PLAYER_2)
			P1Life0->SetRenderOpacity(1.f);
		break;
	}
	case 1:
	{
		if (nPlayer == PLAYER_1)
			P2Life1->SetRenderOpacity(1.f);
		else if (nPlayer == PLAYER_2)
			P1Life1->SetRenderOpacity(1.f);
		break;
	}
	case 2:
	{
		LogWarning(FString::FromInt(nPlayer) + " has all lives!!!");
		break;
	}
	default:
	{
		break;
	}
	}
}
