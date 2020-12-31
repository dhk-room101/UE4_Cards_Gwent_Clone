// Fill out your copyright notice in the Description page of Project Settings.

#include "CardCounter.h"

void UCardCounter::NativeConstruct()
{
	Super::NativeConstruct();

	BorderCard = (UBorder*)GetWidgetFromName(TEXT("BorderCard"));
	BorderCardBackground = (UBorder*)GetWidgetFromName(TEXT("BorderCardBackground"));
	TextCounter = (UTextBlock*)GetWidgetFromName(TEXT("TextCounter"));
}
