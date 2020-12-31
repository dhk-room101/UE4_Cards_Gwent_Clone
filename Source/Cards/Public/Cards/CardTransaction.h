// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "CardTransaction.generated.h"

class UCardInstance;

/**
 *
 */
UCLASS()
class CARDS_API UCardTransaction : public UObject
{
	GENERATED_BODY()

public:
	UCardTransaction(const FObjectInitializer& ObjectInitializer);

	UPROPERTY()
		UCardInstance* sourceCardInstanceRef = nullptr;
	UPROPERTY()
		UCardInstance* targetCardInstanceRef = nullptr;
	
	UPROPERTY()
		int32 targetSlotID;
	UPROPERTY()
		int32 targetPlayerID;
	
	UPROPERTY()
		float powerChangeResult = 0.f;
	UPROPERTY()
		float strategicValue = 0.f;

	FString GetInfo();
};
