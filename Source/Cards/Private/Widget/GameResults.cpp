// Fill out your copyright notice in the Description page of Project Settings.


#include "GameResults.h"
#include "DeckBuilder.h"
#include "CardsPlayerController.h"
#include "CardsGameMain.h"
#include "CardsCardHolder.h"
#include "Border.h"
#include "TextBlock.h"
#include "CanvasPanel.h"
#include "Widget.h"
#include "UniformGridPanel.h"
#include "ldf.h"

void UGameResults::NativeConstruct()
{
	Super::NativeConstruct();

	ContinueBG = (UBorder*)GetWidgetFromName(TEXT("ContinueBG"));
	FName BinderFunctionShowHand(TEXT("HandleContinue"));
	ContinueBG->OnMouseButtonUpEvent.BindUFunction(this, BinderFunctionShowHand);
	ContinueText = (UTextBlock*)GetWidgetFromName(TEXT("ContinueText"));
	ContinueText->SetVisibility(ESlateVisibility::HitTestInvisible);

	GameResultText = (UTextBlock*)GetWidgetFromName(TEXT("GameResultText"));
	PlayerText = (UTextBlock*)GetWidgetFromName(TEXT("PlayerText"));
	OpponentText = (UTextBlock*)GetWidgetFromName(TEXT("OpponentText"));

	Round1 = (UTextBlock*)GetWidgetFromName(TEXT("Round1"));
	Round2 = (UTextBlock*)GetWidgetFromName(TEXT("Round2"));
	Round3 = (UTextBlock*)GetWidgetFromName(TEXT("Round3"));

	PR1 = (UTextBlock*)GetWidgetFromName(TEXT("PR1"));
	PR2 = (UTextBlock*)GetWidgetFromName(TEXT("PR2"));
	PR3 = (UTextBlock*)GetWidgetFromName(TEXT("PR3"));

	OR1 = (UTextBlock*)GetWidgetFromName(TEXT("OR1"));
	OR2 = (UTextBlock*)GetWidgetFromName(TEXT("OR2"));
	OR3 = (UTextBlock*)GetWidgetFromName(TEXT("OR3"));

	ResetLayout();
}

void UGameResults::HandleContinue()
{
	//reset stuff
	SetVisibility(ESlateVisibility::Hidden);
	ResetLayout();

	GetCurrentPlayerController()->UICardsGameMain->P1Life0->SetRenderOpacity(0.33f);
	GetCurrentPlayerController()->UICardsGameMain->P1Life1->SetRenderOpacity(0.33f);
	GetCurrentPlayerController()->UICardsGameMain->P2Life0->SetRenderOpacity(0.33f);
	GetCurrentPlayerController()->UICardsGameMain->P2Life1->SetRenderOpacity(0.33f);
	GetCurrentPlayerController()->UICardsGameMain->P1LeaderDisable->SetVisibility(ESlateVisibility::Hidden);
	GetCurrentPlayerController()->UICardsGameMain->P2LeaderDisable->SetVisibility(ESlateVisibility::Hidden);
	GetCurrentPlayerController()->UICardsGameMain->P1PassedText->SetVisibility(ESlateVisibility::Hidden);
	GetCurrentPlayerController()->UICardsGameMain->P2PassedText->SetVisibility(ESlateVisibility::Hidden);
	GetCurrentPlayerController()->UICardsGameMain->P1CardsHandText->SetText(FText::FromString("0"));
	GetCurrentPlayerController()->UICardsGameMain->P2CardsHandText->SetText(FText::FromString("0"));
	GetCurrentPlayerController()->UICardsGameMain->SetVisibility(ESlateVisibility::Hidden);

	UCanvasPanel* canvasRootGameMain = Cast<UCanvasPanel>(GetCurrentPlayerController()->UICardsGameMain->GetRootWidget());
	for (int32 i = 0; i < canvasRootGameMain->GetChildrenCount(); i++)
	{
		UWidget* _childHolder = canvasRootGameMain->GetChildAt(i);
		if (_childHolder && _childHolder->IsA(UCardsCardHolder::StaticClass()))
		{
			UCardsCardHolder* _holder = Cast<UCardsCardHolder>(_childHolder);
			_holder->ClearChildren();
		}
	}

	GetCurrentPlayerController()->UIDeckBuilder->SetVisibility(ESlateVisibility::Visible);
	GetCurrentPlayerController()->UIDeckBuilder->StartDeckBuilder();
}

void UGameResults::ResetLayout()
{
	PlayerText->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));//WHITE
	OpponentText->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	GameResultText->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	PR1->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	PR2->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	PR3->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	OR1->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	OR2->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	OR3->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	PR1->SetText(FText::FromString("0"));
	PR2->SetText(FText::FromString("0"));
	PR3->SetText(FText::FromString("0"));
	OR1->SetText(FText::FromString("0"));
	OR2->SetText(FText::FromString("0"));
	OR3->SetText(FText::FromString("0"));
}
