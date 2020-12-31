#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/WeakObjectPtr.h"

class UCardInstance;

#include "STypes.generated.h"

USTRUCT()
struct FFSMState
{
	GENERATED_BODY()

	UPROPERTY()
		int32 nState = 0;
	UPROPERTY()
		UFunction* enterStateCallback = nullptr;
	UPROPERTY()
		UFunction* updateStateCallback = nullptr;
	UPROPERTY()
		UFunction* leaveStateCallback = nullptr;

	FORCEINLINE bool operator== (const FFSMState& Other)
	{
		return nState == Other.nState;
	}
};

USTRUCT()
struct FCardAndPointsCombo
{
	GENERATED_BODY()

	UPROPERTY()
		UCardInstance* cardInstance = nullptr;
	UPROPERTY()
		int32 comboPoints = 0;

	FORCEINLINE bool operator== (const FCardAndPointsCombo& Other)
	{
		return cardInstance == Other.cardInstance 
			&& comboPoints == Other.comboPoints;
	}
};