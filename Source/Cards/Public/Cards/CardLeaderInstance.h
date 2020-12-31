// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CardInstance.h"
#include "CardLeaderInstance.generated.h"

class UCardManager;

/**
 *
 */
UCLASS()
class CARDS_API UCardLeaderInstance : public UCardInstance
{
	GENERATED_BODY()

public:
	UCardLeaderInstance(const FObjectInitializer& ObjectInitializer);
	
	virtual void FinalizeSetup() override;
	virtual void RecalculatePowerPotential(UCardManager* cardManager) override;

	void ApplyLeaderAbility(bool bIsAI);

	UPROPERTY()
		int32 nLeaderEffect;
	UPROPERTY()
		bool bCanBeUsed = true;

protected:
	void ClearWeather();
	void ApplyClearWeather();
	void ApplyHorn(int32 nCardListLocation, int32 nPlayer);
	void Scorch(int32 nCardType);
	void ShowEnemyHand(int32 nCardCount);
	void HandleResurrectChoice(int32 nCardInstanceID = -1);
	void ResurrectGraveyard(int32 nPlayer);
	void BinPick(int32 nBin, int32 nPick);
	void PickWeather(bool bIsAI);

	UPROPERTY()
		int32 nNumToBin;
	UPROPERTY()
		int32 nNumBinnedSoFar;
	UPROPERTY()
		int32 nNumToPick;
	UPROPERTY()
		int32 nNumPickedSoFar;

};
