// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CardInstance.h"

#include "UObject/NoExportTypes.h"
#include "CardEffectManager.generated.h"

/**
 *
 */
UCLASS()
class CARDS_API UCardEffectManager : public UObject
{
	GENERATED_BODY()

public:
	UCardEffectManager(const FObjectInitializer& ObjectInitializer);

	TArray<UCardInstance*> GetEffectsForList(int32 nTargetSlot, int32 nPlayer);
	TArray<UCardInstance*>& GetEffectList(int32 nCardListLocation, int32 nPlayer);

	void RegisterActiveEffectCardInstance(UCardInstance* cardInstance, int32 nCardListLocation, int32 nPlayer);
	void UnregisterActiveEffectCardInstance(UCardInstance* cardInstance);

	void FlushAllEffects();

private:
	TArray<UCardInstance*> siegeP2EffectsList;
	TArray<UCardInstance*> rangedP2EffectsList;
	TArray<UCardInstance*> meleeP2EffectsList;
	
	TArray<UCardInstance*> meleeP1EffectsList;
	TArray<UCardInstance*> rangedP1EffectsList;
	TArray<UCardInstance*> siegeP1EffectsList;
};
