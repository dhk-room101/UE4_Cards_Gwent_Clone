// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CardsSingleton.h"

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CardsLibrary.generated.h"

/**
 * 
 */
UCLASS()
class CARDS_API UCardsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UCardsLibrary(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = Singleton)
		static UCardsSingleton* GetCardsData(bool& IsValid);
	
	
	
};
