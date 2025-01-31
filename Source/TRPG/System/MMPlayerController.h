// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMPlayerController.generated.h"

// Forward declarations
class UMMHUDWidget;
class UMySaveGame;

/**
 * 
 */
UCLASS()
class TRPG_API AMMPlayerController : public APlayerController
{
	GENERATED_BODY()
	AMMPlayerController();

protected:
	// PlayerHUD widget
	TObjectPtr<UMMHUDWidget> HUDWidget;

public:
	// Hud Widget to Spawn in world
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HUDWidgetClass;

	void ShowPlayerMatchesRecordData(UMySaveGame* LoadedData);
};
