// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardTemplate.h"
#include "CardInstance.h"
#include "DeckBuilder.generated.h"

class UUniformGridPanel;
class UCardManager;
class UCardsDeck;
class UCardSlot;
class UCardsSaveGame;
class UBorder;
class UTextBlock;
class UCardsCardHolder;

/**
 *
 */
UCLASS()
class CARDS_API UDeckBuilder : public UUserWidget
{
	GENERATED_BODY()

public:
	UUniformGridPanel* HolderAll;//From
	UUniformGridPanel* HolderDeck;//To
	UBorder* ButtonStart;
	UTextBlock* TextArrow;
	UTextBlock* TextCardCounter;
	UCardsCardHolder* ZoomHolder;
	UBorder* BorderAccept;//Add/Remove
	UBorder* BorderCancel;
	UTextBlock* TextAccept;
	UTextBlock* TextCancel;

	UPROPERTY()
		UCardSlot* transactionCardSlot = nullptr;

	void StartDeckBuilder();
	void SpawnCardInstance(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer);
	void WasAddedToList(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer);
	UCardSlot* GetCardSlotFromCardInstance(UCardInstance* cardInstance);
	UCardsCardHolder* GetCardHolder(int32 nCardListLocation, int32 nPlayer);
	void HandleEffectDescription(bool bHide);
	void HandleTransactionButtons(bool bHide);
	void SaveGame();

	UPROPERTY()
		UCardManager* cardManager;

	//keep this order for game save
	TMap<int32, int32> CardsHolderAll;//index, qty
	TMap<int32, int32> CardsHolderDeck;//index, qty
	int32 nFactionLeaderP1;

private:
	void SetCardTemplates();
	void SetLeaderCardTemplates();

	void SetupNewGame(int32 nTemp);

	void AddIndicesToDeck(int32 nIndex);
	void HandleCardSlot(UCardSlot& cardSlot, int32 nCardListLocation, int32 nPlayer);
	void SetCardIndeces();
	void LoadCardsFromSave(UCardsSaveGame* LoadGameInstance);
	void UpdateTextCardCounter();
	void RearrangeSlots(UUniformGridPanel* HolderGrid);
	void ResetGrid();

	UPROPERTY()
		UCardsCardHolder* destinationCardHolder = nullptr;

	int32 maxDeckCardPower;

	TArray<UCardsDeck*> decks;

	inline static bool PowerSorterTemplate(const UCardTemplate& card1, const UCardTemplate& card2)
	{
		return card1.nPower < card2.nPower;//ascending
	}

	inline static bool PowerSorterInstance(const UCardInstance& card1, const UCardInstance& card2)
	{
		return card1.templateRef->nPower > card2.templateRef->nPower;//descending
	}

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void TransactionAccept();
	UFUNCTION()
		void TransactionCancel();
	UFUNCTION()
		void StartGame();
};
