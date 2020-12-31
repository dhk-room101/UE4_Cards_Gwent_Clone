// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CardSlot.generated.h"

class UBorder;
class UTextBlock;
class UCardInstance;
class UCardDescription;
class UCardCounter;

/**
 *
 */
UCLASS()
class CARDS_API UCardSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void UpdateCardPowerText();

	UBorder* BorderPicture;
	UBorder* BorderFrame;
	UBorder* BorderPower;
	UBorder* BorderType;
	UBorder* BorderEffect;
	UBorder* BorderFaction;
	UBorder* BorderPowerHero;

	UTextBlock* TextPower;
	
	UBorder* BorderCard;
	UBorder* BorderCardBackground;
	UTextBlock* TextCounter;

	UCardDescription* CardDescription;
	UCardCounter* CardCounter;

//protected:
	UPROPERTY()
		UCardInstance* cardInstanceRef = nullptr;

//private:
	UPROPERTY()
		int32 nCardIndex = -1;
	UPROPERTY()
		int32 nInstanceId = -1;
	
protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
};
