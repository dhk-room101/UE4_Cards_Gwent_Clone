// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BasePlayerController.h"
#include "HumanPlayerController.generated.h"

class UCardsCardHolder;

/**
 *
 */
UCLASS()
class CARDS_API UHumanPlayerController : public UBasePlayerController
{
	GENERATED_BODY()

public:
	UHumanPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void StartTurn() override;
	virtual void SkipTurn() override;

	void HandleHolderChosen(UCardsCardHolder* cardHolder);

protected:

	UFUNCTION()
		void State_about_to_update();//common

	UFUNCTION()
		void State_begin_Idle();
	UFUNCTION()
		void State_leave_Idle();

	UFUNCTION()
		void State_begin_ChoosingMove();
	UFUNCTION()
		void State_update_ChoosingMove();
	UFUNCTION()
		void State_leave_ChoosingMove();

	UFUNCTION()
		void State_begin_ChoosingHandler();
	UFUNCTION()
		void State_update_ChoosingHandler();

	UFUNCTION()
		void State_begin_ChoosingTargetCard();
	UFUNCTION()
		void State_update_ChoosingTargetCard();

	UFUNCTION()
		void State_begin_WaitConfirmation();
	UFUNCTION()
		void State_update_WaitConfirmation();

	virtual void State_begin_ApplyingCard() override;
	virtual void State_update_ApplyingCard() override;

private:
	UPROPERTY()
		bool bPlayerTurnMessageSeen = false;
};
