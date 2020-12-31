// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "CardTemplate.generated.h"

/**
 *
 */
UCLASS()
class CARDS_API UCardTemplate : public UObject
{
	GENERATED_BODY()

public:
	UCardTemplate(const FObjectInitializer& ObjectInitializer);

	bool IsType(int32 nType);
	bool HasEffect(int32 nEffect);

	int32 GetFirstEffect();
	int32 GetPlacementType();
	int32 GetEffectsAsPlacementType();
	int32 GetCreatureType();

	FString GetTypeString();
	FString GetPlacementTypeString();
	FString GetEffectString();
	FString GetTooltipIcon();
	FString GetTooltipString();
	FString GetFactionString();
	FString ToString();

	float GetBonusValue();


	UPROPERTY()
		int32 nIndex;
	UPROPERTY()
		int32 nPower;
	UPROPERTY()
		int32 nFactionIdx;
	UPROPERTY()
		int32 nTypeArray;
	
	UPROPERTY()
		FString sImageLoc;
	UPROPERTY()
		FString sTitle;
	UPROPERTY()
		FString sDescription;
	
	TArray<int32> effectFlags;
	TArray<int32> summonFlags;
};
