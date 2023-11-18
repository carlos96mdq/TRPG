// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TRPGPlayerState.generated.h"

// Forward declarations
class ABaseUnit;
class ATerrain;
class APlayerStash;
enum class UnitState : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitSelected, ABaseUnit*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChanged, UnitState)

/**
 * 
 */
UCLASS()
class TRPG_API ATRPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Reference to the terrain class
	ATerrain* Terrain;

	// Reference to the unit playing in this turn. It is always dependent of the GameState
	ABaseUnit* ActiveUnit;

	// Own unit selected at the moment
	ABaseUnit* SelectedUnit;

public:


	// Player stash where saves all items that are not equipped in any unit
	APlayerStash* PlayerStash;

	// Set a reference to the terrain class created by the GameState
	void SetTerrain(ATerrain* TerrainPointer);
	
	// Set a reference to the unit playing in this turn and modify the HUD
	void SetActiveUnit(ABaseUnit* NewActiveUnit);

	// Process the HitResult received from the PlayerController
	void ProcessClick(FHitResult& HitResult);

	// Try to reverse the current state to Idle or unselected if possible
	void ReverseState(UUserWidget* CombatPlayerWidget);

	// Change the CurrentState
	void ChangeState(UnitState NewState, int32 ActionPosition = -1);

	// Called by widget when move button is pressed
	void CheckMovement(UUserWidget* CombatPlayerWidget);

	// Delegate declaration
	FOnUnitSelected OnUnitSelected;
	FOnStateChanged OnStateChanged;

	ABaseUnit* GetActiveUnit() const { return ActiveUnit; };
	ABaseUnit* GetSelectedUnit() const { return SelectedUnit; };
};
