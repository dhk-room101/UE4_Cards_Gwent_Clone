// Fill out your copyright notice in the Description page of Project Settings.

#include "Utils/FiniteStateMachine.h"
#include "ldf.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"

UFiniteStateMachine::UFiniteStateMachine(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UFiniteStateMachine::StartTimerCheckUpdates()
{
	GetCurrentWorld()->GetFirstPlayerController()->GetWorldTimerManager().SetTimer(TimerHandleCheckUpdates, this, &UFiniteStateMachine::UpdateStates, 0.20f, true);
}

void UFiniteStateMachine::UpdateStates()
{
	if (nNextState != nCurrentState && disallowStateChangeFunc)
	{
		return;
	}
	if (nNextState != nCurrentState && stateList.Contains(nNextState))
	{

		LogWarning(ownerRef->GetName() + " | GFX - [FSM] Switching from: " + ConvertStateToString(nCurrentState) + ", to: " + ConvertStateToString(nNextState));



		if (stateList.Contains(nCurrentState) && stateList[nCurrentState].leaveStateCallback)
		{
			void* locals = nullptr;

			ownerRef->ProcessEvent(stateList[nCurrentState].leaveStateCallback, locals);
		}
		prevStateName = nCurrentState;
		nCurrentState = nNextState;
		if (stateList.Contains(nNextState) && stateList[nNextState].enterStateCallback)
		{
			void* locals = nullptr;

			ownerRef->ProcessEvent(stateList[nCurrentState].enterStateCallback, locals);
		}
	}
	if (nCurrentState == STATE_INVALID)
	{
		return;
	}
	if (stateList[nCurrentState].updateStateCallback)
	{
		void* locals = nullptr;

		ownerRef->ProcessEvent(stateList[nCurrentState].updateStateCallback, locals);
	}
}

void UFiniteStateMachine::AddState(int32 nState, UFunction* enterStateCallback, UFunction* updateStateCallback, UFunction* leaveStateCallback)
{
	FFSMState fsm;
	fsm.nState = nState;
	fsm.enterStateCallback = enterStateCallback;
	fsm.updateStateCallback = updateStateCallback;
	fsm.leaveStateCallback = leaveStateCallback;

	stateList.Add(nState, fsm);


	if (nCurrentState == STATE_INVALID && nNextState == STATE_INVALID)
	{
		nNextState = nState;
	}
}

void UFiniteStateMachine::ChangeState(int32 nState)
{
	FString sState = GetStateFromInt(nState);
	LogWarning("[[FSM]] Changing state to " + sState);

	bool bHasNextState = stateList.Contains(nState);

	if (bHasNextState)
	{
		nNextState = nState;
	}

	else
	{
		LogWarning(ownerRef->GetName() + " | GFX - [WARNING] Tried to change to an unknown state: " + FString::FromInt(nState));
	}

}

int32 UFiniteStateMachine::GetCurrentState()
{
	return nCurrentState;
}

FString UFiniteStateMachine::GetStateFromInt(int32 nState)
{
	switch (nState)
	{
	case STATE_INVALID: return "STATE_INVALID";
	case STATE_INITIALIZING: return "STATE_INITIALIZING";
	case STATE_TUTORIALS: return "STATE_TUTORIALS";
	case STATE_SPAWNLEADERS: return "STATE_SPAWNLEADERS";
	case STATE_COINTOSS: return "STATE_COINTOSS";
	case STATE_MULLIGAN: return "STATE_MULLIGAN";
	case STATE_ROUNDSTART: return "STATE_ROUNDSTART";
	case STATE_PLAYERTURN: return "STATE_PLAYERTURN";
	case STATE_CHANGINGPLAYER: return "STATE_CHANGINGPLAYER";
	case STATE_SHOWINGROUNDRESULT: return "STATE_SHOWINGROUNDRESULT";
	case STATE_CLEARINGBOARD: return "STATE_CLEARINGBOARD";
	case STATE_SHOWINGFINALRESULT: return "STATE_SHOWINGFINALRESULT";
	case STATE_RESET: return "STATE_RESET";
	case STATE_IDLE: return "STATE_IDLE";
	case STATE_CHOOSINGMOVE: return "STATE_CHOOSINGMOVE";
	case STATE_CHOOSINGHANDLER: return "STATE_CHOOSINGHANDLER";
	case STATE_CHOOSINGTARGETCARD: return "STATE_CHOOSINGTARGETCARD";
	case STATE_WAITCONFIRMATION: return "STATE_WAITCONFIRMATION";
	case STATE_APPLYINGCARD: return "STATE_APPLYINGCARD";
	case STATE_SENDINGCARDTOTRANSACTION: return "STATE_SENDINGCARDTOTRANSACTION";
	case STATE_DELAYBETWEENACTIONS: return "STATE_DELAYBETWEENACTIONS";
	case STATE_ROUNDPASSED: return "STATE_ROUNDPASSED";
	case STATE_ABILITY_NILFGAARD: return "STATE_ABILITY_NILFGAARD";
	case STATE_ABILITY_NORTHERN: return "STATE_ABILITY_NORTHERN";
	case STATE_ABILITY_MONSTERS: return "STATE_ABILITY_MONSTERS";
	case STATE_ABILITY_SKOIATAEL: return "STATE_ABILITY_SKOIATAEL";
	case STATE_COINTOSS_OPPONENT: return "STATE_COINTOSS_OPPONENT";
	case STATE_COINTOSS_PLAYER: return "STATE_COINTOSS_PLAYER";
	case STATE_PLAYERTURN_OPPONENT: return "STATE_PLAYERTURN_OPPONENT";
	case STATE_PLAYERTURN_PLAYER: return "STATE_PLAYERTURN_PLAYER";
	case STATE_ROUNDLOST: return "STATE_ROUNDLOST";
	case STATE_ROUNDWON: 	return "STATE_ROUNDWON";
	default:
		break;
	}

	return "STATE_INVALID";
}

