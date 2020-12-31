// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CardsGameMain.generated.h"

class UTextBlock;
class UBorder;
class UImage;
class UUserWidget;
class UCardsCardHolder;

class UCardInstance;
class UCardSlot;

/**
 * 
 */
UCLASS()
class CARDS_API UCardsGameMain : public UUserWidget
{
	GENERATED_BODY()

public:
	UCardsCardHolder* Weather;
	UCardsCardHolder* ZoomHolder;
	UCardsCardHolder* P1Hand;
	UCardsCardHolder* P1HandBackground;
	UCardsCardHolder* P2HandInvisible;
	UCardsCardHolder* P1Siege;
	UCardsCardHolder* P2Siege;
	UCardsCardHolder* P1Ranged;
	UCardsCardHolder* P2Ranged;
	UCardsCardHolder* P1Melee;
	UCardsCardHolder* P2Melee;

	UCardsCardHolder* P1SiegeModifier;
	UCardsCardHolder* P2SiegeModifier;
	UCardsCardHolder* P1RangedModifier;
	UCardsCardHolder* P2RangedModifier;
	UCardsCardHolder* P1MeleeModifier;
	UCardsCardHolder* P2MeleeModifier;

	UCardsCardHolder* P1Graveyard;
	UCardsCardHolder* P2Graveyard;

	UCardsCardHolder* P1Deck;
	UCardsCardHolder* P2Deck;

	UCardsCardHolder* P1Leader;
	UCardsCardHolder* P2Leader;

	UBorder* P1LeaderUse;
	UBorder* P2LeaderUse;
	UBorder* P1LeaderDisable;
	UBorder* P2LeaderDisable;

	UBorder* P1Life0;
	UBorder* P1Life1;
	UBorder* P2Life0;
	UBorder* P2Life1;

	UBorder* P1DeckBorder;
	UBorder* P2DeckBorder;

	UTextBlock* P1DeckText;
	UTextBlock* P2DeckText;

	UTextBlock* P1CardsHandText;
	UTextBlock* P2CardsHandText;

	UTextBlock* P1ScoreAllText;
	UTextBlock* P2ScoreAllText;

	UTextBlock* P1ScoreSiegeText;
	UTextBlock* P2ScoreSiegeText;

	UTextBlock* P1ScoreRangedText;
	UTextBlock* P2ScoreRangedText;

	UTextBlock* P1ScoreMeleeText;
	UTextBlock* P2ScoreMeleeText;

	UTextBlock* P1PassedText;
	UTextBlock* P2PassedText;

	UTextBlock* P1FactionText;
	UTextBlock* P2FactionText;

	UBorder* BorderAccept;
	UBorder* BorderCancel;
	UTextBlock* TextAccept;
	UTextBlock* TextCancel;

	UBorder* BorderSkipTurn;
	UTextBlock* TextSkipTurn;

	UBorder* BorderShowHand;
	UTextBlock* TextShowHand;
	bool bShowHand = false;

	UBorder* BorderStateBackground;
	UBorder* BorderState;
	UTextBlock* TextState;
	
	UImage* FrameRow;
	UImage* FrameWeather;

	UPROPERTY()
		UCardSlot* transactionCardSlot = nullptr;

	void SpawnCardInstance(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer);
	void WasAddedToList(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer);

	void UpdateDeckCounter();
	void UpdateScore(int32 nP1Score, int32 nP2Score);
	void UpdateRowScore(int32 nP1Siege, int32 nP1Ranged, int32 nP1Melee, int32 nP2Melee, int32 nP2Ranged, int32 nP2Siege);
	void UpdateCardsInHand(int32 nPlayer, int32 nCardsCount);
	void UpdateFactionText();
	void UpdateFactionDeckCover();

	void HandleTransactionButtons(bool bHide, UCardInstance* cardInstance = nullptr);
	void HandleEffectDescription(bool bHide);
	void HandleSkipTurnButton(bool bHide);
	void ClearCardTransactionZoom();
	void HandleChangeStates(int32 nState, int32 nPlayer, bool bHide);
	void ActivateHoldersForCard(UCardInstance* cardInstance, bool bHide);
	void RemoveCardFromHolder(UCardInstance* cardInstance);
	void PlaySound(FString sSound);
	
	void RearrangeSlots(UCardsCardHolder* cardHolder);

	void DisableLeaderUse(int32 nPlayer);
	void SetPlayerLives(int32 nPlayer, int32 nLives);

	UCardSlot* GetCardSlotFromCardInstance(UCardInstance* cardInstance);
	UCardsCardHolder* GetCardHolder(int32 nCardListLocation, int32 nPlayer);

	void HandleCardSlot(UCardSlot& cardSlot, int32 nCardListLocation, int32 nPlayer);

protected:
	virtual void NativeConstruct() override;
	
	UFUNCTION()
		void TransactionAccept();
	UFUNCTION()
		void TransactionCancel();
	UFUNCTION()
		void HandleSkipTurn();
	UFUNCTION()
		void HandleShowHand();

	UPROPERTY()
		UCardsCardHolder* destinationCardHolder = nullptr;
};
