// Fill out your copyright notice in the Description page of Project Settings.

#include "CardsLibrary.h"

#include "Engine/Engine.h"

UCardsLibrary::UCardsLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//get singleton
UCardsSingleton* UCardsLibrary::GetCardsData(bool& IsValid)
{
	IsValid = false;
	UCardsSingleton* DataInstance = Cast<UCardsSingleton>(GEngine->GameSingleton);

	if (!DataInstance) return NULL;
	if (!DataInstance->IsValidLowLevel()) return NULL;

	IsValid = true;
	return DataInstance;
}



