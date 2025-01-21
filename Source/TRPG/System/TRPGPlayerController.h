// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TRPGPlayerController.generated.h"

// Forward declarations
class ABaseUnit;
class UActiveUnitWidget;
class UHUDWidget;
class UUnitDataIcon;
class UInputMappingContext;
class UInputAction;
struct FInputActionInstance;
enum class EUnitControllerOwner : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnActiveUnitSet, ABaseUnit*)

/**
 * 
 */
UCLASS()
class TRPG_API ATRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

	bool bIsPlayerTurn = false;

	int32 CameraHMovement = 0;
	int32 CameraHMovementMax = 100;
	int32 CameraVMovement = 0;
	int32 CameraVMovementMax = 120;

	// Widget that show the data of the active unit widget
	UActiveUnitWidget* ActiveUnitWidget;
	
	// PlayerHUD widget
	UHUDWidget* HUDWidget;

	TMap<int32, UUnitDataIcon*> UnitDataIconList;

	int32 PlayerActiveUnitIndex = 0;

	// MainArena Camera	--TODO esto deberia ver donde ponerlo, lo mejor creo que seria spawnearlo en el PlayerState, ahora lo dejo aca para probar facilmente
	ACameraActor* MainCamera;

	EUnitControllerOwner ControllerOwnerName;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	// The left click is used to select the active unit or see data about an enemy unit
	void OnMouseLeftClicked();

	// The right click is used to perform an action with an active unit, as move, attack or any else
	void OnMouseRightClicked();

	// This function is called at the start of a match. It should check for all the units in the maps, identify the player's one
	// and register each one, creating an UUnitDataIcon for them in the process
	void RegisterAllUnits();

	void NewTurnStarts(EUnitControllerOwner ControllerTurn);

	void SetActiveUnit(ABaseUnit* NewActiveUnit);

	void GameOver(EUnitControllerOwner WinnerController);

public:
	ATRPGPlayerController();

	// Allows the PlayerController to set up custom input bindings
	virtual void SetupInputComponent() override;

	virtual void OnUnitStops(int32 PlayerUnitIndex);

	// Called by the active unit when finishing with its moving process so the tiles could be paint again
	void OnUnitUpdateStats(ABaseUnit* Unit);

	// Called by action widget when a Combat action was selected and pressed
	void OnCombatAction(int32 ActionPosition);
	
	// Called by action widget when Move action is pressed
	void OnMoveAction();
	
	// Called by action widget when EndTurn action is pressed
	void OnEndTurnAction();

	void OnRestartAction();

	// Called when players press action to move camera
	void OnMoveCameraAction(const FInputActionInstance& Instance);

	// Called when players press action to zoom camera
	void OnZoomCameraAction(const FInputActionInstance& Instance);

	EUnitControllerOwner GetControllerName() const { return ControllerOwnerName; }

	FOnActiveUnitSet OnActiveUnitSet;

	// User Widget class used to spawn widgets in the world
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> UserWidgetClass;

	// CombatPlayer Widget to Spawn in world
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> ActiveUnitWidgetClass;

	// CombatPlayer Widget to Spawn in world
	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widget")
	TSubclassOf<UUnitDataIcon> UnitDataIconClass;

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
