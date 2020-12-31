// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CardTransaction.h"
#include "CardTemplate.h"

#include "UObject/NoExportTypes.h"
#include "CardInstance.generated.h"

class UCardManager;

/**
 *
 */
UCLASS()
class CARDS_API UCardInstance : public UObject
{
	GENERATED_BODY()

public:
	UCardInstance(const FObjectInitializer& ObjectInitializer);

	inline static bool PowerChangeSorter(const UCardInstance& card1, const UCardInstance& card2)
	{
		if (card1.GetOptimalTransaction()->powerChangeResult == card2.GetOptimalTransaction()->powerChangeResult)
		{
			return card1.GetOptimalTransaction()->strategicValue < card2.GetOptimalTransaction()->strategicValue;
		}
		return (card1.GetOptimalTransaction()->powerChangeResult < card2.GetOptimalTransaction()->powerChangeResult);
	}

	virtual void FinalizeSetup();
	virtual void RecalculatePowerPotential(UCardManager* cardManager);

	void OnFinishedMovingIntoHolder(int32 nCardHolder, int32 nPlayer);
	void PowerChangeCallback();
	void UpdateEffectsApplied();

	int32 GetTotalPower(bool bHasEffect = false);
	int32 GetNotListPlayer();
	int32 GetNotOwningPlayer();
	int32 PotentialWeatherHarm();

	UCardTransaction* GetOptimalTransaction() const;

	bool CanBeCastOn(UCardInstance* cardInstance);
	bool CanBePlacedInSlot(int32 nCardListLocation, int32 nPlayer);

	int32 nOwningPlayer;
	int32 nInList;
	int32 nInstanceId;
	int32 nListsPlayer;
	int32 nLastListApplied;
	int32 nLastListPlayerApplied;
	int32 nTemplateId;

	TArray<UCardInstance*> effectingCardsRefList;
	TArray<UCardInstance*> effectedByCardsRefList;

	UPROPERTY()
		UCardTemplate* templateRef = nullptr;

protected:
	UPROPERTY()
		UCardTransaction* _lastCalculatedPowerPotential = nullptr;

	void AddToEffectingList(UCardInstance* cardInstance);
	void AddEffect(UCardInstance* cardInstance);
	void RemoveFromEffectingList(UCardInstance* cardInstance);
	void RemoveEffect(UCardInstance* cardInstance);
	void SummonClones(UCardInstance* cardInstance);
	void ResurrectCard(UCardInstance* cardInstance = nullptr);
	void HandleNurseChoice(int32 nInstanceID);
};
