// Fill out your copyright notice in the Description page of Project Settings.

#include "DeckBuilder.h"
#include "CardsSaveGame.h"
#include "CardsGameInstance.h"
#include "CardsCardHolder.h"
#include "CardsCardValues.h"
#include "CardsDeck.h"
#include "CardsGameFlowController.h"
#include "CardsGameMain.h"
#include "CardManager.h"
#include "CardSlot.h"
#include "CardLeaderInstance.h"
#include "CardCounter.h"
#include "CardDescription.h"
#include "ldf.h"
#include "XmlParser.h"
#include "UniformGridPanel.h"
#include "WidgetBlueprintLibrary.h"
#include "Border.h"
#include "TextBlock.h"
#include "UniformGridSlot.h"
#include "CanvasPanel.h"
#include "WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WidgetTree.h"
#include "Engine/Engine.h"
#include "ScrollBox.h"

void UDeckBuilder::NativeConstruct()
{
	Super::NativeConstruct();

	HolderAll = (UUniformGridPanel*)GetWidgetFromName(TEXT("HolderAll"));
	HolderDeck = (UUniformGridPanel*)GetWidgetFromName(TEXT("HolderDeck"));
	TextCardCounter = (UTextBlock*)GetWidgetFromName(TEXT("TextCardCounter"));

	TextArrow = (UTextBlock*)GetWidgetFromName(TEXT("TextArrow"));
	ButtonStart = (UBorder*)GetWidgetFromName(TEXT("ButtonStart"));
	FName BinderFunctionStart(TEXT("StartGame"));
	ButtonStart->OnMouseButtonUpEvent.BindUFunction(this, BinderFunctionStart);

	ZoomHolder = (UCardsCardHolder*)GetWidgetFromName(TEXT("ZoomHolder"));
	ZoomHolder->nCardHolder = CARD_LIST_LOC_ZOOM;
	ZoomHolder->nPlayer = PLAYER_1;
	ZoomHolder->sTag = "ZoomHolder";

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
}

void UDeckBuilder::StartDeckBuilder()
{
	ResetGrid();

	CardsHolderAll.Empty();
	CardsHolderDeck.Empty();
	decks.Empty();

	cardManager = NewObject<UCardManager>();
	cardManager->cardValues = NewObject<UCardsCardValues>();
	cardManager->_cardTemplates.Empty();

	GetCurrentPlayerController()->IsDeckBuilder = true;
	GetGameFlowController()->playerControllers[PLAYER_1]->bInputEnabled = true;

	for (int32 i = 0; i < 3; i++)//TODO hardcoded no. of decks, currently Neutral, Paragon1, Paragon2
	{
		UCardsDeck* deck = NewObject<UCardsDeck>();
		deck->AddToRoot();
		decks.Add(deck);
	}

	FString sString;
	UCardsSaveGame* LoadGameInstance = Cast<UCardsSaveGame>(UGameplayStatics::CreateSaveGameObject(UCardsSaveGame::StaticClass()));
	LoadGameInstance = Cast<UCardsSaveGame>(UGameplayStatics::LoadGameFromSlot("CardsSaveSlot", 0));

	if (LoadGameInstance)
	{
		nFactionLeaderP1 = LoadGameInstance->nFactionLeaderP1;
		sString = "Deck Builder: Save found! Leader set to " + FString::FromInt(nFactionLeaderP1);

		//set wins/loses here too
		GetCurrentGameInstance()->nWins = LoadGameInstance->nWins;
		GetCurrentGameInstance()->nLoses = LoadGameInstance->nLoses;
		GetCurrentGameInstance()->nDraws = LoadGameInstance->nDraws;
	}
	else
	{
		sString = "Deck Builder: No Save found for leader!";

		//temp manually
		SetupNewGame(0);/*set faction leader*/
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

	SetLeaderCardTemplates();
	SetCardTemplates();

	cardManager->playerDeckDefinitions[PLAYER_1]->nSelectedKingIndex = nFactionLeaderP1;

	if (LoadGameInstance)
	{
		//nFactionLeaderP1 = LoadGameInstance->nFactionLeaderP1;
		sString = "Deck Builder: Save found! Cards set from save!";

		LoadCardsFromSave(LoadGameInstance);

		UpdateTextCardCounter();
		SetCardIndeces();
	}
	else
	{
		sString = "Deck Builder: No Save found for cards";

		//temp manually
		SetupNewGame(1);/*set human card indices*/
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);
}

void UDeckBuilder::SpawnCardInstance(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer)
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
		if (GetCurrentPlayerController()->wCardSlotDeckBuilder)
		{
			cardSlot = CreateWidget<UCardSlot>(GetCurrentPlayerController(), GetCurrentPlayerController()->wCardSlotDeckBuilder);
		}
	}

	cardSlot->cardInstanceRef = cardInstance;
	cardSlot->nInstanceId = cardInstance->nInstanceId;
	cardSlot->nCardIndex = cardInstance->templateRef->nIndex;

	HandleCardSlot(*cardSlot, nCardListLocation, nPlayer);

	if (nCardListLocation != CARD_LIST_LOC_ALL && nCardListLocation != CARD_LIST_LOC_DECKBUILDER)
		WasAddedToList(cardInstance, nCardListLocation, nPlayer);
}

void UDeckBuilder::WasAddedToList(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer)
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

UCardSlot* UDeckBuilder::GetCardSlotFromCardInstance(UCardInstance* cardInstance)
{
	UCanvasPanel* canvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);

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

UCardsCardHolder* UDeckBuilder::GetCardHolder(int32 nCardListLocation, int32 nPlayer)
{
	UCanvasPanel* canvas = Cast<UCanvasPanel>(WidgetTree->RootWidget);

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

void UDeckBuilder::HandleEffectDescription(bool bHide)
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

void UDeckBuilder::HandleTransactionButtons(bool bHide)
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
		BorderAccept->SetVisibility(ESlateVisibility::Visible);
		TextAccept->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UDeckBuilder::SetCardTemplates()
{
	FString sCards = FPaths::GameDevelopersDir() + "Source/XML/" + "def_cards_cards_final.xml";

	const FXmlFile fileCards(sCards);

	const FXmlNode* redXmlNode = fileCards.GetRootNode();

	const TArray<FXmlNode*> cardDefinitions = redXmlNode->GetChildrenNodes();

	for (int32 i = 0; i < cardDefinitions.Num(); i++)
	{
		UCardTemplate* _template = NewObject<UCardTemplate>();
		_template->AddToRoot();

		FXmlNode* _node = cardDefinitions[i];
		const TArray<FXmlNode*> children = _node->GetChildrenNodes();
		const TArray<FXmlAttribute> attributes = _node->GetAttributes();

		FXmlNode* _typeNode = children[0]; //type flags
		const TArray<FXmlNode*> typeFlags = _typeNode->GetChildrenNodes();

		for (int32 t = 0; t < typeFlags.Num(); t++)
		{
			FXmlNode* _typeFlag = typeFlags[t];
			const TArray<FXmlAttribute> typeAttributes = _typeFlag->GetAttributes();

			int32 nType = ConvertTypeStringToInt(typeAttributes[0].GetValue());

			_template->nTypeArray |= nType;
		}

		FXmlNode* _effectNode = children[1]; //effect flags
		const TArray<FXmlNode*> effectFlags = _effectNode->GetChildrenNodes();

		for (int32 e = 0; e < effectFlags.Num(); e++)
		{
			FXmlNode* _effectFlag = effectFlags[e];
			const TArray<FXmlAttribute> effectAttributes = _effectFlag->GetAttributes();

			int32 nEffect = ConvertEffectStringToInt(effectAttributes[0].GetValue());

			_template->effectFlags.Add(nEffect);
		}

		if (children.Num() == 3) //summon flags
		{
			FXmlNode* _summonNode = children[2];
			const TArray<FXmlNode*> summonFlags = _summonNode->GetChildrenNodes();

			for (int32 s = 0; s < summonFlags.Num(); s++)
			{
				FXmlNode* _summonFlag = summonFlags[s];
				const TArray<FXmlAttribute> summonAttributes = _summonFlag->GetAttributes();

				int32 nsummon = FCString::Atoi(*summonAttributes[0].GetValue());

				_template->summonFlags.Add(nsummon);
			}
		}

		_template->nIndex = FCString::Atoi(*_node->GetAttribute("index"));
		_template->nPower = FCString::Atoi(*_node->GetAttribute("power"));
		_template->nFactionIdx = ConvertFactionStringToInt(_node->GetAttribute("faction_index"));

		_template->sImageLoc = _node->GetAttribute("picture");
		_template->sTitle = _node->GetAttribute("title");
		_template->sDescription = _node->GetAttribute("description");

		cardManager->_cardTemplates.Add(_template->nIndex, _template);

		AddIndicesToDeck(_template->nIndex);
	}
}

void UDeckBuilder::SetLeaderCardTemplates()
{
	FString sCards = FPaths::GameDevelopersDir() + "Source/XML/" + "def_cards_battle_king_cards.xml";

	const FXmlFile fileCards(sCards);

	const FXmlNode* redXmlNode = fileCards.GetRootNode();

	const TArray<FXmlNode*> cardDefinitions = redXmlNode->GetChildrenNodes();

	for (int32 i = 0; i < cardDefinitions.Num(); i++)
	{
		UCardTemplate* _template = NewObject<UCardTemplate>();
		_template->AddToRoot();

		FXmlNode* _node = cardDefinitions[i];
		const TArray<FXmlNode*> children = _node->GetChildrenNodes();
		const TArray<FXmlAttribute> attributes = _node->GetAttributes();

		FXmlNode* _typeNode = children[0]; //type flags
		const TArray<FXmlNode*> typeFlags = _typeNode->GetChildrenNodes();

		for (int32 t = 0; t < typeFlags.Num(); t++)
		{
			FXmlNode* _typeFlag = typeFlags[t];
			const TArray<FXmlAttribute> typeAttributes = _typeFlag->GetAttributes();

			int32 nType = ConvertTypeStringToInt(typeAttributes[0].GetValue());

			_template->nTypeArray |= nType;
		}

		FXmlNode* _effectNode = children[1]; //effect flags
		const TArray<FXmlNode*> effectFlags = _effectNode->GetChildrenNodes();

		for (int32 e = 0; e < effectFlags.Num(); e++)
		{
			FXmlNode* _effectFlag = effectFlags[e];
			const TArray<FXmlAttribute> effectAttributes = _effectFlag->GetAttributes();

			int32 nEffect = ConvertEffectStringToInt(effectAttributes[0].GetValue());

			_template->effectFlags.Add(nEffect);
		}

		if (children.Num() == 3) //summon flags
		{
			FXmlNode* _summonNode = children[2];
			const TArray<FXmlNode*> summonFlags = _summonNode->GetChildrenNodes();

			for (int32 s = 0; s < summonFlags.Num(); s++)
			{
				FXmlNode* _summonFlag = summonFlags[s];
				const TArray<FXmlAttribute> summonAttributes = _summonFlag->GetAttributes();

				int32 nsummon = FCString::Atoi(*summonAttributes[0].GetValue());

				_template->summonFlags.Add(nsummon);
			}
		}

		_template->nIndex = FCString::Atoi(*_node->GetAttribute("index"));
		_template->nPower = FCString::Atoi(*_node->GetAttribute("power"));
		_template->nFactionIdx = ConvertFactionStringToInt(_node->GetAttribute("faction_index"));

		_template->sImageLoc = _node->GetAttribute("picture");
		_template->sTitle = _node->GetAttribute("title");
		_template->sDescription = _node->GetAttribute("description");

		cardManager->_cardTemplates.Add(_template->nIndex, _template);
	}
}

void UDeckBuilder::SetupNewGame(int32 nTemp)
{
	switch (nTemp)
	{
	case 0 /*set faction leader*/:
	{
		nFactionLeaderP1 = 1001;

		break;
	}
	case 1 /*set human card indices*/:
	{
		//GetGameFlowController()->playerControllers[PLAYER_1]->bInputEnabled = true;

		/*CardsHolderDeck.Add(3, 1);
		CardsHolderDeck.Add(4, 1);
		CardsHolderDeck.Add(5, 1);
		CardsHolderDeck.Add(6, 1);*/
		CardsHolderDeck.Add(106, 1);
		CardsHolderDeck.Add(107, 1);
		CardsHolderDeck.Add(108, 2);
		CardsHolderDeck.Add(111, 1);
		CardsHolderDeck.Add(112, 1);
		CardsHolderDeck.Add(113, 1);
		CardsHolderDeck.Add(114, 1);
		CardsHolderDeck.Add(115, 1);
		CardsHolderDeck.Add(116, 1);
		CardsHolderDeck.Add(120, 1);
		CardsHolderDeck.Add(121, 1);
		CardsHolderDeck.Add(125, 3);
		CardsHolderDeck.Add(135, 1);
		CardsHolderDeck.Add(145, 1);
		CardsHolderDeck.Add(146, 1);
		CardsHolderDeck.Add(150, 1);
		CardsHolderDeck.Add(160, 1);
		CardsHolderDeck.Add(175, 1);

		CardsHolderAll.Add(108, 2);
		CardsHolderAll.Add(125, 1);
		CardsHolderAll.Add(113, 1);
		CardsHolderAll.Add(135, 2);
		CardsHolderAll.Add(150, 1);

		UpdateTextCardCounter();
		SetCardIndeces();

		break;
	}
	default:
	{
		break;
	}
	}

}

void UDeckBuilder::AddIndicesToDeck(int32 nIndex)
{
	int32 nDeck = 0;

	if (nIndex < 100)
		nDeck = 0;
	else if (nIndex >= 100 && nIndex < 200)
		nDeck = 1;
	else if (nIndex >= 200 && nIndex < 300)
		nDeck = 2;
	else if (nIndex >= 300 && nIndex < 400)
		nDeck = 3;
	else if (nIndex >= 400)
		nDeck = 4;

	decks[nDeck]->cardIndices.Add(nIndex);
}

void UDeckBuilder::HandleCardSlot(UCardSlot& cardSlot, int32 nCardListLocation, int32 nPlayer)
{
	FString sLocation;

	switch (nCardListLocation)
	{
	case CARD_LIST_LOC_ALL:
	{
		HolderAll->AddChildToUniformGrid(&cardSlot);
		break;
	}
	case CARD_LIST_LOC_DECKBUILDER:
	{
		HolderDeck->AddChildToUniformGrid(&cardSlot);
		break;
	}
	case CARD_LIST_LOC_ZOOM:
	{
		ZoomHolder->AddChildToCanvas(&cardSlot);

		if (transactionCardSlot && transactionCardSlot->CardDescription)
			transactionCardSlot->CardDescription->SetVisibility(ESlateVisibility::HitTestInvisible);

		GetDeckBuilder()->HandleEffectDescription(false);

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

void UDeckBuilder::SetCardIndeces()
{
	//sort
	TArray<UCardTemplate*> _templateList;

	for (const TPair<int32, int32>& pair : CardsHolderDeck)
	{
		_templateList.Add(cardManager->_cardTemplates[pair.Key]);
	}

	_templateList.Sort(PowerSorterTemplate);//ascending

	LogWarning(FString::FromInt(_templateList.Num())
		+ " | first " + FString::FromInt(_templateList[0]->nPower)
		+ " | last " + FString::FromInt(_templateList[_templateList.Num() - 1]->nPower));

	maxDeckCardPower = _templateList[_templateList.Num() - 1]->nPower;

	//add visual card
	UCardInstance* cardInstance = nullptr;

	int32 nRow = 0;
	int32 nColumn = 0;

	TMap<UCardInstance*, int32> cardInstances;

	//Deck/Right
	for (const TPair<int32, int32>& pair : CardsHolderDeck)
	{
		cardInstance = cardManager->SpawnCardInstance(pair.Key, PLAYER_1);
		cardInstances.Add(cardInstance, pair.Value);
	}

	//cardInstances.Sort(PowerSorterInstance);//descending

	for (const TPair<UCardInstance*, int32>& pair : cardInstances)
	{
		cardInstance = pair.Key;
		cardInstance->nLastListApplied = CARD_LIST_LOC_DECKBUILDER;
		cardInstance->nLastListPlayerApplied = PLAYER_1;

		UCardSlot* cardSlot = nullptr;

		if (GetCurrentPlayerController()->wCardSlotDeckBuilder)
		{
			cardSlot = CreateWidget<UCardSlot>(GetCurrentPlayerController(), GetCurrentPlayerController()->wCardSlotDeckBuilder);
		}

		cardSlot->cardInstanceRef = cardInstance;
		cardSlot->nInstanceId = cardInstance->nInstanceId;
		cardSlot->nCardIndex = cardInstance->templateRef->nIndex;

		HandleCardSlot(*cardSlot, CARD_LIST_LOC_DECKBUILDER, PLAYER_1);

		cardSlot->CardCounter->SetVisibility(ESlateVisibility::HitTestInvisible);
		cardSlot->CardCounter->TextCounter->SetText(FText::FromString(FString::FromInt(pair.Value)));

		UUniformGridSlot* slot = UWidgetLayoutLibrary::SlotAsUniformGridSlot(cardSlot);
		slot->SetRow(nRow);
		slot->SetColumn(nColumn);

		nColumn++;

		if (nColumn % 3 == 0)
		{
			nRow++;
			nColumn = 0;
		}

		//TODO sound
	}

	//All/Left
	nRow = 0;
	nColumn = 0;

	cardInstances.Empty();

	for (const TPair<int32, int32>& pair : CardsHolderAll)
	{
		cardInstance = cardManager->SpawnCardInstance(pair.Key, PLAYER_1);
		cardInstances.Add(cardInstance, pair.Value);
	}

	//cardInstances.Sort(PowerSorterInstance);//descending

	for (const TPair<UCardInstance*, int32>& pair : cardInstances)
	{
		cardInstance = pair.Key;
		cardInstance->nLastListApplied = CARD_LIST_LOC_ALL;
		cardInstance->nLastListPlayerApplied = PLAYER_1;

		UCardSlot* cardSlot = nullptr;

		if (GetCurrentPlayerController()->wCardSlotDeckBuilder)
		{
			cardSlot = CreateWidget<UCardSlot>(GetCurrentPlayerController(), GetCurrentPlayerController()->wCardSlotDeckBuilder);
		}

		cardSlot->cardInstanceRef = cardInstance;
		cardSlot->nInstanceId = cardInstance->nInstanceId;
		cardSlot->nCardIndex = cardInstance->templateRef->nIndex;

		HandleCardSlot(*cardSlot, CARD_LIST_LOC_ALL, PLAYER_1);

		cardSlot->CardCounter->SetVisibility(ESlateVisibility::HitTestInvisible);
		cardSlot->CardCounter->TextCounter->SetText(FText::FromString(FString::FromInt(pair.Value)));

		UUniformGridSlot* slot = UWidgetLayoutLibrary::SlotAsUniformGridSlot(cardSlot);
		slot->SetRow(nRow);
		slot->SetColumn(nColumn);

		nColumn++;

		if (nColumn % 3 == 0)
		{
			nRow++;
			nColumn = 0;
		}

		//TODO sound
	}
}

void UDeckBuilder::LoadCardsFromSave(UCardsSaveGame* LoadGameInstance)
{
	for (const TPair<int32, int32>& pair : LoadGameInstance->CardsHolderAll)
	{
		CardsHolderAll.Add(pair.Key, pair.Value);
	}

	for (const TPair<int32, int32>& pair : LoadGameInstance->CardsHolderDeck)
	{
		CardsHolderDeck.Add(pair.Key, pair.Value);
	}

	SetCardIndeces();
}

void UDeckBuilder::UpdateTextCardCounter()
{
	int32 nTotalCards = 0;
	for (const TPair<int32, int32>& pair : CardsHolderDeck)
	{
		nTotalCards += pair.Value;
	}

	FString sString = "Current deck has " + FString::FromInt(nTotalCards) + " cards!";

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

	int32 nColor;

	if (nTotalCards != 25)
	{
		nColor = 0xFF0000;
		ButtonStart->SetVisibility(ESlateVisibility::Hidden);
		TextArrow->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		nColor = 0xFFFFFF;//0xACE1F9;//0xF0C069;
		ButtonStart->SetVisibility(ESlateVisibility::Visible);
		TextArrow->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	float r = ((nColor >> 16) & 255) / 255;
	float g = ((nColor >> 8) & 255) / 255;
	float b = (nColor & 255) / 255;
	float a = 255 / 255;

	TextCardCounter->SetColorAndOpacity(FLinearColor(r, g, b, a));
	TextCardCounter->SetText(FText::FromString(FString::FromInt(nTotalCards)));
}

void UDeckBuilder::RearrangeSlots(UUniformGridPanel* HolderGrid)
{
	return;

	int32 nRow = 0;
	int32 nColumn = 0;

	for (int32 i = 0; i < HolderGrid->GetChildrenCount(); i++)
	{
		UCardSlot* cardSlot = nullptr;

		if (HolderGrid->GetChildAt(i) && HolderGrid->GetChildAt(i)->IsA(UCardSlot::StaticClass()))
		{
			cardSlot = Cast<UCardSlot>(HolderGrid->GetChildAt(i));
		}

		if (cardSlot)
		{
			UUniformGridSlot* slot = UWidgetLayoutLibrary::SlotAsUniformGridSlot(cardSlot);
			slot->SetRow(nRow);
			slot->SetColumn(nColumn);

			nColumn++;

			if (nColumn % 3 == 0)
			{
				nRow++;
				nColumn = 0;
			}
		}
	}
}

void UDeckBuilder::ResetGrid()
{
	UCanvasPanel* canvasRootDeckBuilder = Cast<UCanvasPanel>(GetRootWidget());
	for (int32 i = 0; i < canvasRootDeckBuilder->GetChildrenCount(); i++)
	{
		UWidget* _childHolder = canvasRootDeckBuilder->GetChildAt(i);
		if (_childHolder && _childHolder->IsA(UScrollBox::StaticClass()))
		{
			UScrollBox* _holder = Cast<UScrollBox>(_childHolder);
			{
				for (int32 j = 0; j < _holder->GetChildrenCount(); j++)
				{
					UWidget* _childHolderScroll = _holder->GetChildAt(j);
					if (_childHolderScroll && _childHolderScroll->IsA(UUniformGridPanel::StaticClass()))
					{
						UUniformGridPanel* _holderScroll = Cast<UUniformGridPanel>(_childHolderScroll);
						_holderScroll->ClearChildren();
					}
				}
			}
		}
	}
}

void UDeckBuilder::TransactionAccept()
{
	ResetGrid();

	FString sString;

	int32 nListFrom = transactionCardSlot->cardInstanceRef->nLastListApplied;
	int32 nListTo = -1;

	if (nListFrom == CARD_LIST_LOC_ALL)
		nListTo = CARD_LIST_LOC_DECKBUILDER;
	else if (nListFrom == CARD_LIST_LOC_DECKBUILDER)
		nListTo = CARD_LIST_LOC_ALL;
	else
	{
		TransactionCancel();

		sString = "Wrong ListFrom, instead of 12/13 got " + FString::FromInt(nListFrom);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);
	}

	int32 nIndex = transactionCardSlot->cardInstanceRef->templateRef->nIndex;

	int32 nCounterFrom = nListFrom == CARD_LIST_LOC_ALL ?
		(CardsHolderAll.Contains(nIndex) ? CardsHolderAll[nIndex] : 0)
		:
		(CardsHolderDeck.Contains(nIndex) ? CardsHolderDeck[nIndex] : 0);
	int32 nCounterTo = nListTo == CARD_LIST_LOC_ALL ?
		(CardsHolderAll.Contains(nIndex) ? CardsHolderAll[nIndex] : 0)
		:
		(CardsHolderDeck.Contains(nIndex) ? CardsHolderDeck[nIndex] : 0);

	UUniformGridPanel* HolderFrom = nListFrom == CARD_LIST_LOC_ALL ? HolderAll : HolderDeck;
	UUniformGridPanel* HolderTo = nListTo == CARD_LIST_LOC_ALL ? HolderAll : HolderDeck;

	if (HolderFrom == HolderTo)
	{
		sString = "should be different TO/FROM holders, got SAME!";

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);
	}

	bool bHandled = false;

	if (nCounterFrom == 0)
	{
		sString = "expecting at least qty 1 for template " + FString::FromInt(nIndex) + " from list " + FString::FromInt(nListFrom);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);
	}
	else if (nCounterFrom == 1)
	{
		/*DONE*/if (nCounterTo == 0)
		{
			sString = "Switcheroo MOVE " + FString::FromInt(nIndex);
			
			if (nListFrom == CARD_LIST_LOC_ALL)
			{
				CardsHolderAll.Remove(nIndex);
				CardsHolderDeck.Add(nIndex, 1);
			}
			else
			{
				CardsHolderDeck.Remove(nIndex);
				CardsHolderAll.Add(nIndex, 1);
			}
		}
		/*DONE*/else
		{
			sString = "Prepare to remove template " + FString::FromInt(nIndex) + " from list " + FString::FromInt(nListFrom);

			int32 nNewCounter = 0;

			if (nListFrom == CARD_LIST_LOC_ALL)
			{
				nNewCounter = CardsHolderDeck[nIndex];
				nNewCounter = nNewCounter + 1;
				CardsHolderDeck.Add(nIndex, nNewCounter);
				CardsHolderAll.Remove(nIndex);
			}
			else
			{
				nNewCounter = CardsHolderAll[nIndex];
				nNewCounter = nNewCounter + 1;
				CardsHolderAll.Add(nIndex, nNewCounter);
				CardsHolderDeck.Remove(nIndex);
			}
		}

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

		bHandled = true;
	}

	/*DONE*/if (nCounterTo == 0 && !bHandled)
	{
		sString = "No template " + FString::FromInt(nIndex) + " found in TO list " + FString::FromInt(nListTo) + " , creating new!";

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

		int32 nNewCounter = 0;

		if (nListFrom == CARD_LIST_LOC_ALL)
		{
			nNewCounter = CardsHolderAll[nIndex];
			nNewCounter = nNewCounter - 1;
			CardsHolderAll[nIndex] = nNewCounter;
			CardsHolderDeck.Add(nIndex, 1);
		}
		else
		{
			nNewCounter = CardsHolderDeck[nIndex];
			nNewCounter = nNewCounter - 1;
			CardsHolderDeck[nIndex] = nNewCounter;
			CardsHolderAll.Add(nIndex, 1);
		}
	}

	/*DONE*/if (nCounterFrom > 1 && nCounterTo > 0)
	{
		sString = "Simple COPY of template " + FString::FromInt(nIndex);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

		int32 nNewCounterFrom = 0;
		if (nListFrom == CARD_LIST_LOC_ALL)
		{
			nNewCounterFrom = CardsHolderAll[nIndex];
			nNewCounterFrom = nNewCounterFrom - 1;
			CardsHolderAll[nIndex] = nNewCounterFrom;
		}
		else
		{
			nNewCounterFrom = CardsHolderDeck[nIndex];
			nNewCounterFrom = nNewCounterFrom - 1;
			CardsHolderDeck[nIndex] = nNewCounterFrom;
		}

		int32 nNewCounterTo = 0;
		if (nListTo == CARD_LIST_LOC_ALL)
		{
			nNewCounterTo = CardsHolderAll[nIndex];
			nNewCounterTo = nNewCounterTo + 1;
			CardsHolderAll[nIndex] = nNewCounterTo;
		}
		else
		{
			nNewCounterTo = CardsHolderDeck[nIndex];
			nNewCounterTo = nNewCounterTo + 1;
			CardsHolderDeck[nIndex] = nNewCounterTo;
		}
	}

	UpdateTextCardCounter();
	SetCardIndeces();

	//remove zoom
	TransactionCancel();
}

void UDeckBuilder::TransactionCancel()
{
	//clear zoom card
	HandleTransactionButtons(true);
	ZoomHolder->RemoveChildAt(0);
	transactionCardSlot = nullptr;
}

void UDeckBuilder::StartGame()
{
	FString sString = "START GAME!";

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

	SaveGame();

	SetVisibility(ESlateVisibility::Hidden);
	GetBoardRenderer()->SetVisibility(ESlateVisibility::Visible);

	GetCurrentGameInstance()->StartGame();
}

void UDeckBuilder::SaveGame()
{
	FString sString;

	UCardsSaveGame* SaveGameInstance = Cast<UCardsSaveGame>(UGameplayStatics::CreateSaveGameObject(UCardsSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		for (const TPair<int32, int32>& pair : CardsHolderAll)
		{
			SaveGameInstance->CardsHolderAll.Add(pair.Key, pair.Value);
		}

		for (const TPair<int32, int32>& pair : CardsHolderDeck)
		{
			SaveGameInstance->CardsHolderDeck.Add(pair.Key, pair.Value);
		}

		SaveGameInstance->nFactionLeaderP1 = nFactionLeaderP1;
		SaveGameInstance->nWins = GetCurrentGameInstance()->nWins;
		SaveGameInstance->nLoses = GetCurrentGameInstance()->nLoses;
		SaveGameInstance->nDraws = GetCurrentGameInstance()->nDraws;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("CardsSaveSlot"), 0);
	}
	else
	{
		sString = "Failed Save!!!";

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

		return;
	}
}
