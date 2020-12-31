// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsGameModeBase.h"

#include "CardsLibrary.h"

ACardsGameModeBase::ACardsGameModeBase()
{

}

//reset singleton when PIE ends
void ACardsGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::EndPlayInEditor)
	{
		bool IsValid;
		UCardsLibrary::GetCardsData(IsValid)->CardsCurrentWorld = nullptr;
	}
}


