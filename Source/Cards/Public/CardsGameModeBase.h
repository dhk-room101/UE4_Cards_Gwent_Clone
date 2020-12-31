// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"
#include "CardsGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class CARDS_API ACardsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACardsGameModeBase();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	
	
};
