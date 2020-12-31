// Fill out your copyright notice in the Description page of Project Settings.

#include "CardSlot.h"

#include "CardsGameFlowController.h"
#include "CardsGameMain.h"
#include "CardsCardHolder.h"
#include "DeckBuilder.h"

#include "CardInstance.h"
#include "CardTemplate.h"
#include "CardManager.h"
#include "CardDescription.h"
#include "CardCounter.h"

#include "ldf.h"
#include "STypes.h"

#include "CardsLibrary.h"

#include "UMG.h"
#include "WidgetBlueprintLibrary.h"

void UCardSlot::NativeConstruct()
{
	Super::NativeConstruct();

	BorderPicture = (UBorder*)GetWidgetFromName(TEXT("BorderPicture"));
	BorderFrame = (UBorder*)GetWidgetFromName(TEXT("BorderFrame"));
	BorderPower = (UBorder*)GetWidgetFromName(TEXT("BorderPower"));
	BorderType = (UBorder*)GetWidgetFromName(TEXT("BorderType"));
	BorderEffect = (UBorder*)GetWidgetFromName(TEXT("BorderEffect"));
	BorderFaction = (UBorder*)GetWidgetFromName(TEXT("BorderFaction"));
	BorderPowerHero = (UBorder*)GetWidgetFromName(TEXT("BorderPowerHero"));//TODO

	TextPower = (UTextBlock*)GetWidgetFromName(TEXT("TextPower"));

	CardDescription = (UCardDescription*)GetWidgetFromName(TEXT("WidgetCardDescription"));
	if (CardDescription)
		CardDescription->SetVisibility(ESlateVisibility::Hidden);

	CardCounter = (UCardCounter*)GetWidgetFromName(TEXT("WidgetCardCounter"));
	if (CardCounter)
		CardCounter->SetVisibility(ESlateVisibility::Hidden);
}

FReply UCardSlot::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (GetGameFlowController()->playerControllers[PLAYER_1]->bInputEnabled)
	{
		if (GetCurrentPlayerController()->IsDeckBuilder)
		{
			if (GetDeckBuilder()->ZoomHolder->GetChildrenCount() == 0)
			{
				GetBoardRenderer()->PlaySound("DealCard");

				GetDeckBuilder()->SpawnCardInstance(cardInstanceRef, CARD_LIST_LOC_ZOOM, PLAYER_1);
				GetDeckBuilder()->HandleTransactionButtons(false);

				return FReply::Handled();
			}
		}
		else
		{
			if (GetBoardRenderer()->ZoomHolder->GetChildrenCount() == 0)
			{
				GetBoardRenderer()->PlaySound("DealCard");

				GetBoardRenderer()->SpawnCardInstance(cardInstanceRef, CARD_LIST_LOC_ZOOM, PLAYER_1);
				GetBoardRenderer()->HandleTransactionButtons(false, cardInstanceRef);
				//GetBoardRenderer()->HandleEffectDescription(false);
				this->SetVisibility(ESlateVisibility::Hidden);

				return FReply::Handled();
			}
		}
	}

	return FReply::Unhandled();
}

void UCardSlot::UpdateCardPowerText()
{
	int32 nTotalPower = 0;

	UCardInstance* cardInstance = nullptr;
	UCardTemplate* cardTemplate = GetCardManager()->GetCardTemplate(nCardIndex);

	if (TextPower)
	{
		if (nInstanceId == -1)
		{
			TextPower->SetText(FText::FromString(FString::FromInt(cardTemplate->nPower)));
		}
		else
		{
			cardInstance = GetCardManager()->GetCardInstance(nInstanceId);
			nTotalPower = cardInstance->GetTotalPower();
			TextPower->SetText(FText::FromString(FString::FromInt(nTotalPower)));
		}

		if (cardTemplate->IsType(CardType_Creature))
		{
			TextPower->SetVisibility(ESlateVisibility::HitTestInvisible);

			if (nInstanceId != -1 && cardInstance->templateRef->nPower < nTotalPower)
			{
				//nColor = 2195475;//green 21 80 13
				TextPower->SetColorAndOpacity(FLinearColor(0.13f, 0.5f, 0.075f, 1.f));
			}
			else if (nInstanceId != -1 && cardInstance->templateRef->nPower > nTotalPower)
			{
				//nColor = 12648448;//red C1 00 00
				TextPower->SetColorAndOpacity(FLinearColor(0.75f, 0.f, 0.f, 1.f));
			}
			else if (cardTemplate->IsType(CardType_Hero))
			{
				//nColor = 16777215;//white
				TextPower->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
			}
			else
			{
				//nColor = 0;//black
				TextPower->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
			}
		}
		else
		{
			TextPower->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}