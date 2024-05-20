// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TRPGPlayerController.generated.h"

// Forward declarations
class ABaseUnit;
class UActiveUnitWidget;
class UHUDWidget;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;

/**
 * 
 */
UCLASS()
class TRPG_API ATRPGPlayerController : public APlayerController
{
	GENERATED_BODY()
	ATRPGPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	// Called when mouse left button is clicked
	// This action take the actor under the mouse cursor and send the data to PlayerState to procces an action
	void OnMouseLeftClicked();
	
	// Called when mouse right button is clicked
	// This action is a "reverse" one, so it say the PlayerState to reverse its current state
	void OnMouseRightClicked();

	// Widget that show the data of the active unit widget
	UActiveUnitWidget* ActiveUnitWidget;
	
	// PlayerHUD widget
	UHUDWidget* HUDWidget;

	// MainArena Camera	--TODO esto deberia ver donde ponerlo, lo mejor creo que seria spawnearlo en el PlayerState, ahora lo dejo aca para probar facilmente
	ACameraActor* MainCamera;

public:
	// Allows the PlayerController to set up custom input bindings
	virtual void SetupInputComponent() override;

	// Called by action widget when a Combat action was selected and pressed
	void OnCombatAction(int32 ActionPosition);
	
	// Called by action widget when Move action is pressed
	void OnMoveAction();
	
	// Called by action widget when Wait action is pressed
	void OnWaitAction();

	// Called when players press action to move camera
	void OnMoveCameraAction(const FInputActionInstance& Instance);

	// Called when players press action to zoom camera
	void OnZoomCameraAction(const FInputActionInstance& Instance);

	// User Widget class used to spawn widgets in the world
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> UserWidgetClass;

	// CombatPlayer Widget to Spawn in world
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ActiveUnitWidgetClass;

	// CombatPlayer Widget to Spawn in world
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> HUDWidgetClass;

	// Input Mapping Context asset for the Enhanced Input System
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	// Input Action to move the camera
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> MoveCameraInputAction;

	// Input Action to zoom the camera
	UPROPERTY(EditAnywhere, Category = "Input")
	TSoftObjectPtr<UInputAction> ZoomCameraInputAction;
};
