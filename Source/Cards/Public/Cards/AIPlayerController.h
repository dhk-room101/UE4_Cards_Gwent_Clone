// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CardInstance.h"

#include "BasePlayerController.h"
#include "AIPlayerController.generated.h"

class UCardTransaction;

/**
 *
 */
UCLASS()
class CARDS_API UAIPlayerController : public UBasePlayerController
{
	GENERATED_BODY()

public:
	UAIPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void StartTurn() override;
	virtual void SkipTurn() override;

	inline static bool PowerChangeSorter(const UCardInstance& card1, const UCardInstance& card2)
	{
		if (card1.GetOptimalTransaction()->powerChangeResult == card2.GetOptimalTransaction()->powerChangeResult)
		{
			return card1.GetOptimalTransaction()->strategicValue < card2.GetOptimalTransaction()->strategicValue;
		}
		return (card1.GetOptimalTransaction()->powerChangeResult < card2.GetOptimalTransaction()->powerChangeResult);
	}

	inline static bool StrategicValueSorter(const UCardInstance& card1, const UCardInstance& card2)
	{
		return card1.GetOptimalTransaction()->strategicValue < card2.GetOptimalTransaction()->strategicValue;
	}

protected:
	UPROPERTY()
		bool bCurrentRoundCritical = false;

	UPROPERTY()
		int32 nAttitude;
	UPROPERTY()
		int32 nChances;

	UFUNCTION()
		void State_begin_Idle();
	UFUNCTION()
		void State_leave_Idle();

	UFUNCTION()
		void State_begin_ChoosingMove();
	UFUNCTION()
		void State_update_ChoosingMove();

	UFUNCTION()
		void State_begin_SendingCardToTransaction();
	UFUNCTION()
		void State_update_SendingCardToTransaction();

	UFUNCTION()
		void State_begin_DelayBetweenActions();
	UFUNCTION()
		void State_update_DelayBetweenActions();

	UCardTransaction* DecideWhichCardToPlay();

	UCardInstance* GetHighestValueCardOnBoard();
	UCardInstance* GetHighestValueCardOnBoardWithEffectLessThan(int32 nDifference);

	TArray<UCardInstance*> GetCardsBasedOnCriteria(int32 nSort);

private:
	void ChooseAttitude();
};
