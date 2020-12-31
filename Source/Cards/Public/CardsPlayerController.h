// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "CardsPlayerController.generated.h"

class UCardSlot;
class USoundCue;

/**
 * 
 */
UCLASS()
class CARDS_API ACardsPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ACardsPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UCardsGameMain> wUICardsGameMain;

	UPROPERTY()
		UCardsGameMain* UICardsGameMain = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UDeckBuilder> wUIDeckBuilder;

	UPROPERTY()
		UDeckBuilder* UIDeckBuilder = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UGameResults> wUIGameResults;

	UPROPERTY()
		UGameResults* UIGameResults = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UCardSlot> wCardSlotBoard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UCardSlot> wCardSlotZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<class UCardSlot> wCardSlotDeckBuilder;

	TMap<int32, UTexture2D*> statesTextures;
	TMap<int32, UTexture2D*> factionDeckTextures;
	TMap<int32, UTexture2D*> factionBannerTextures;
	TMap<int32, UTexture2D*> typeTextures;
	TMap<int32, UTexture2D*> effectTextures; 
	TMap<int32, UTexture2D*> picTextures;
	
	TMap<FString, USoundCue*> sounds;
	
	bool IsDeckBuilder = false;
	FString LAST_MESSAGE;

protected:
	virtual void BeginPlay() override;
	void InitializeUI();
	void LoadTextures();
	void LoadSounds();
};
