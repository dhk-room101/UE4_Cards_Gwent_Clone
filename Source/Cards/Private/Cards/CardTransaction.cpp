// Fill out your copyright notice in the Description page of Project Settings.

#include "CardTransaction.h"

#include "CardInstance.h"
#include "CardTemplate.h"

#include "ldf.h"

UCardTransaction::UCardTransaction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	targetSlotID = CARD_LIST_LOC_INVALID;
	targetPlayerID = PLAYER_INVALID;
}


FString UCardTransaction::GetInfo()
{
	if (targetCardInstanceRef)
		return (
			"[Cards CardTransaction] sourceCard:[[[" + sourceCardInstanceRef->templateRef->sTitle +
			"]]], targetSlotID:" + FString::FromInt(targetSlotID) +
			", targetPlayerID:" + FString::FromInt(targetPlayerID) +
			", StrategicValue:" + FString::SanitizeFloat(strategicValue) +
			", PowerChangeResult:" + FString::SanitizeFloat(powerChangeResult) +
			", targetCardRef:[[[" + targetCardInstanceRef->GetName() + "]]]");
	else
		return (
			"[Cards CardTransaction] sourceCard:[[[" + sourceCardInstanceRef->templateRef->sTitle +
			"]]], targetSlotID:" + FString::FromInt(targetSlotID) +
			", targetPlayerID:" + FString::FromInt(targetPlayerID) +
			", StrategicValue:" + FString::SanitizeFloat(strategicValue) +
			", PowerChangeResult:" + FString::SanitizeFloat(powerChangeResult));
}
