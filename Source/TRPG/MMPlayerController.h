// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MMPlayerController.generated.h"

// Forward declarations
class UMMHUDWidget;


/**
 * 
 */
UCLASS()
class TRPG_API AMMPlayerController : public APlayerController
{
	GENERATED_BODY()
	AMMPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// PlayerHUD widget
	UMMHUDWidget* HUDWidget;

public:
	// Hud Widget to Spawn in world
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HUDWidgetClass;

	// Allows the PlayerController to set up custom input bindings
	virtual void SetupInputComponent() override;

	void ShowPlayerMatchesRecordData(const TArray<int32>& PlayerMatchesRecordData);
};
