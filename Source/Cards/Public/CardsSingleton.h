// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StreamableManager.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "CardsSingleton.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class CARDS_API UCardsSingleton : public UObject
{
	GENERATED_BODY()
	
	UCardsSingleton(const FObjectInitializer& ObjectInitializer);

public:
		FStreamableManager AssetLoader;

	UPROPERTY()
		UWorld* CardsCurrentWorld = nullptr;
	
	
};
