// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CardDescription.generated.h"

class UBorder;
class UTextBlock;

/**
 *
 */
UCLASS()
class CARDS_API UCardDescription : public UUserWidget
{
	GENERATED_BODY()

public:
	UBorder* BorderCard;
	UTextBlock* TextCardDescription;
	UTextBlock* TextCardTitle;

	UBorder* EffectBackground;
	UBorder* EffectTypeBorder;
	UTextBlock* EffectTitle;
	UTextBlock* EffectDescription;

protected:
	virtual void NativeConstruct() override;
};
