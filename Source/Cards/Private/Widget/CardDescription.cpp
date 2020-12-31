// Fill out your copyright notice in the Description page of Project Settings.

#include "CardDescription.h"
#include "Border.h"
#include "TextBlock.h"

void UCardDescription::NativeConstruct()
{
	Super::NativeConstruct();

	BorderCard = (UBorder*)GetWidgetFromName(TEXT("BorderCard"));
	TextCardDescription = (UTextBlock*)GetWidgetFromName(TEXT("TextCardDescription"));
	TextCardTitle = (UTextBlock*)GetWidgetFromName(TEXT("TextCardTitle"));

	EffectBackground = (UBorder*)GetWidgetFromName(TEXT("EffectBackground"));
	EffectTypeBorder = (UBorder*)GetWidgetFromName(TEXT("EffectTypeBorder"));
	EffectTitle = (UTextBlock*)GetWidgetFromName(TEXT("EffectTitle"));
	EffectDescription = (UTextBlock*)GetWidgetFromName(TEXT("EffectDescription"));
}
