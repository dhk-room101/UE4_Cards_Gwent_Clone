// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameResults.generated.h"

class UBorder;
class UTextBlock;

/**
 * 
 */
UCLASS()
class CARDS_API UGameResults : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//UBorder* Background;
	//UBorder* GameResultTextBorder;
	UTextBlock* GameResultText;
	UTextBlock* Round1;
	UTextBlock* Round2;
	UTextBlock* Round3;
	UTextBlock* PlayerText;
	UTextBlock* OpponentText;
	UTextBlock* PR1;
	UTextBlock* PR2;
	UTextBlock* PR3;
	UTextBlock* OR1;
	UTextBlock* OR2;
	UTextBlock* OR3;
	UBorder* ContinueBG;
	UTextBlock* ContinueText;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
		void HandleContinue();

private:
	void ResetLayout();
};
