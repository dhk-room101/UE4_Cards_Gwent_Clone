// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CardCounter.generated.h"

class UBorder;
class UTextBlock;

/**
 *
 */
UCLASS()
class CARDS_API UCardCounter : public UUserWidget
{
	GENERATED_BODY()

public:

	UBorder* BorderCard;
	UBorder* BorderCardBackground;
	UTextBlock* TextCounter;

protected:
	virtual void NativeConstruct() override;
};
