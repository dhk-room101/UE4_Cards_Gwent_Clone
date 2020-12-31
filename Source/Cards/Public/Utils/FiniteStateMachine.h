// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

#include "STypes.h"

#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "FiniteStateMachine.generated.h"

/**
 *
 */
UCLASS()
class CARDS_API UFiniteStateMachine : public UObject
{
	GENERATED_BODY()

public:
	void StartTimerCheckUpdates();

	UFUNCTION()
		void UpdateStates();
	UFUNCTION()
		void AddState(int32 nState, UFunction* enterStateCallback, UFunction* updateStateCallback, UFunction* leaveStateCallback);
	UFUNCTION()
		void ChangeState(int32 nState);

	int32 GetCurrentState();

	TMap<int32, FFSMState> stateList;

	UPROPERTY()
		UObject* ownerRef = nullptr;

protected:
	UFiniteStateMachine(const FObjectInitializer& ObjectInitializer);

private:
	UPROPERTY()
		FTimerHandle TimerHandleCheckUpdates;

	int32 nCurrentState;
	int32 nNextState;
	int32 prevStateName;

	UPROPERTY()
		UFunction* disallowStateChangeFunc;

	FString GetStateFromInt(int32 nState);
};
