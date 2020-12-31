// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsGameInstance.h"
#include "CardsSaveGame.h"

#include "CardsGameFlowController.h"
#include "CardManager.h"
#include "CardsDeck.h"
#include "CardsCardValues.h"
#include "CardsGameMain.h"

#include "ldf.h"

#include "FiniteStateMachine.h"

#include "XmlParser.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UCardsGameInstance::UCardsGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	gameFlowController = NewObject<UCardsGameFlowController>();
}

void UCardsGameInstance::StartGame()
{
	gameFlowController = NewObject<UCardsGameFlowController>();
	
	GetCurrentPlayerController()->IsDeckBuilder = false;

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
		sString = "Game Instance: Save found! Leader set to " + FString::FromInt(nFactionLeaderP1);

		nFactionLeaderP1 = LoadGameInstance->nFactionLeaderP1;
		nFactionLeaderP2 = 2001;
	}
	else
	{
		sString = "WRONG!!! Game Instance: No Save found for leader!";

		//temp manually
		TempFunction(0);/*set faction leader*/
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

	cardManager = NewObject<UCardManager>();
	cardManager->cardValues = NewObject<UCardsCardValues>();
	//cardManager->InitializeLists();

	SetLeaderCardTemplates();
	SetCardTemplates();

	cardManager->playerDeckDefinitions[PLAYER_1]->nSelectedKingIndex = nFactionLeaderP1;

	if (LoadGameInstance)
	{
		nFactionLeaderP1 = LoadGameInstance->nFactionLeaderP1;
		sString = "Game Instance: Save found! Cards set from save!";

		LoadCardsFromSave(LoadGameInstance);
	}
	else
	{
		sString = "WRONG!!! Game Instance: No Save found for cards";

		//temp manually
		TempFunction(1);/*set human card indices*/
	}

	cardManager->playerDeckDefinitions[PLAYER_2]->nSelectedKingIndex = nFactionLeaderP2;
	TempFunction(2);/*set AI card indices*/

	//check power
	int nPlayer1Power = 0;
	for (int32 i = 0; i < cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Num(); i++)
	{
		nPlayer1Power += cardManager->GetCardTemplate(cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices[i])->nPower;
	}

	int nPlayer2Power = 0;
	for (int32 i = 0; i < cardManager->playerDeckDefinitions[PLAYER_2]->cardIndices.Num(); i++)
	{
		nPlayer2Power += cardManager->GetCardTemplate(cardManager->playerDeckDefinitions[PLAYER_2]->cardIndices[i])->nPower;
	}

	sString = "Deck Strength| P1 Power: " + FString::FromInt(nPlayer1Power) + " vs. " + "P2 Power: " + FString::FromInt(nPlayer2Power);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, sString);

	InitializeBoard();

	gameFlowController->stateMachine->StartTimerCheckUpdates();

	//Manual start FSM?
	gameFlowController->playerControllers[PLAYER_1]->GetFiniteStateMachine()->StartTimerCheckUpdates();
	gameFlowController->playerControllers[PLAYER_2]->GetFiniteStateMachine()->StartTimerCheckUpdates();
}

void UCardsGameInstance::SetCardTemplates()
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

void UCardsGameInstance::SetLeaderCardTemplates()
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

void UCardsGameInstance::TempFunction(int32 nTemp)
{
	switch (nTemp)
	{
	case 0 /*set faction leader*/:
	{
		nFactionLeaderP1 = 1001;
		nFactionLeaderP2 = 2001;

		break;
	}
	case 1 /*set human card indices*/:
	{
		/*cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(3);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(4);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(5);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(6);*/
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(106);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(107);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(108);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(111);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(112);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(113);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(114);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(115);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(116);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(120);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(121);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(125);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(135);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(145);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(146);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(150);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(160);
		cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(175);

		//sort
		TArray<UCardTemplate*> _templateList;

		for (int32 i = 0; i < cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Num(); i++)
		{
			_templateList.Add(cardManager->_cardTemplates[cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices[i]]);
		}

		_templateList.Sort(PowerSorter);

		LogWarning(FString::FromInt(_templateList.Num())
			+ " | first " + FString::FromInt(_templateList[0]->nPower)
			+ " | last " + FString::FromInt(_templateList[_templateList.Num() - 1]->nPower));

		maxDeckCardPower = _templateList[_templateList.Num() - 1]->nPower;

		break;
	}
	case 2 /*set AI card indices*/:
	{
		TArray<int32> tempDeckIndicesP1;
		for (int32 i = 0; i < cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Num(); i++)
		{
			tempDeckIndicesP1.Add(cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices[i]);
		}

		UCardsDeck* tempDeckP2 = decks[cardManager->playerDeckDefinitions[PLAYER_2]->GetDeckFaction()];

		TArray<int32> tempDeckIndicesP2;
		for (int32 i = 0; i < tempDeckP2->cardIndices.Num(); i++)
		{
			tempDeckIndicesP2.Add(tempDeckP2->cardIndices[i]);
		}

		int32 c = tempDeckIndicesP1.Num();

		while (c > 0)
		{
			int32 n1 = tempDeckIndicesP1[0];
			if (n1 < 100) /*neutral/shared*/
			{
				cardManager->playerDeckDefinitions[PLAYER_2]->cardIndices.Add(n1);
				tempDeckIndicesP1.RemoveAt(0);
			}
			else
			{
				UCardTemplate* _t1 = cardManager->_cardTemplates[n1];
				bool found = false;
				while (!found)
				{
					TArray<UCardTemplate*> matchPower;

					for (int32 n = 0; n < tempDeckIndicesP2.Num(); n++)
					{
						UCardTemplate* _t2 = cardManager->_cardTemplates[tempDeckIndicesP2[n]];

						if (_t2->nPower == _t1->nPower && !matchPower.Contains(_t2))
							matchPower.Add(_t2);
					}

					while (!found)
					{
						int32 nRandom = FMath::RandRange(0, matchPower.Num() - 1);

						UCardTemplate* _t2 = matchPower[nRandom];

						if (_t2->effectFlags[0] == _t1->effectFlags[0] == CardEffect_None)
						{
							if (_t2->nPower == _t1->nPower)
							{
								cardManager->playerDeckDefinitions[PLAYER_2]->cardIndices.Add(_t2->nIndex);
								LogWarning("AI Deck: POWER AI Card power " + FString::FromInt(_t2->nPower) + " vs. Human Card power " + FString::FromInt(_t1->nPower));

								tempDeckIndicesP1.RemoveAt(0);

								found = true;
							}
						}
						else if (_t2->effectFlags[0] == _t1->effectFlags[0])
						{
							if (_t2->nPower <= maxDeckCardPower)
							{
								cardManager->playerDeckDefinitions[PLAYER_2]->cardIndices.Add(_t2->nIndex);
								LogWarning("AI Deck: EFFECT AI Card power " + FString::FromInt(_t2->nPower) + " vs. Human Card power " + FString::FromInt(_t1->nPower));

								tempDeckIndicesP1.RemoveAt(0);

								found = true;
							}
						}

						if (found)
						{
							break;
						}
					}
				}
			}

			c = tempDeckIndicesP1.Num();
		}

		break;
	}
	default:
	{
		break;
	}
	}

}

void UCardsGameInstance::AddIndicesToDeck(int32 nIndex)
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

void UCardsGameInstance::InitializeBoard()
{
	GetBoardRenderer()->UpdateScore(0, 0);
	GetBoardRenderer()->UpdateRowScore(0, 0, 0, 0, 0, 0);

	GetBoardRenderer()->UpdateFactionText();
	GetBoardRenderer()->UpdateFactionDeckCover();
}

void UCardsGameInstance::LoadCardsFromSave(UCardsSaveGame* LoadGameInstance)
{
	for (const TPair<int32, int32>& pair : LoadGameInstance->CardsHolderDeck)
	{
		for (int32 i = 0; i < pair.Value; i++)
		{
			cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Add(pair.Key);
		}
	}

	//sort
	TArray<UCardTemplate*> _templateList;

	for (int32 i = 0; i < cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices.Num(); i++)
	{
		_templateList.Add(cardManager->_cardTemplates[cardManager->playerDeckDefinitions[PLAYER_1]->cardIndices[i]]);
	}

	_templateList.Sort(PowerSorter);

	LogWarning(FString::FromInt(_templateList.Num())
		+ " | first " + FString::FromInt(_templateList[0]->nPower)
		+ " | last " + FString::FromInt(_templateList[_templateList.Num() - 1]->nPower));

	maxDeckCardPower = _templateList[_templateList.Num() - 1]->nPower;
}

