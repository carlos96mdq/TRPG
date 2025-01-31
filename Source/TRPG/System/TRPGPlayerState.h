// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TRPGPlayerState.generated.h"

// Forward declarations
class ABaseUnit;
enum class EUnitState : uint8;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUnitSelected, ABaseUnit*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChanged, EUnitState)

/**
 * 
 */
UCLASS()
class TRPG_API ATRPGPlayerState : public APlayerState
{
	GENERATED_BODY()
	
protected:
	virtual void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Reference to the unit playing in this turn. It is always dependent of the GameState
	ABaseUnit* ActiveUnit;

	// Own unit selected at the moment
	ABaseUnit* SelectedUnit;

	struct PlayerData
	{
		int32 MatchesPlayed = 0;
		int32 MatchesWon = 0;
		int32 MatchesLost = 0;
		int32 UnitsDefeated = 0;
		int32 UnitsLost = 0;
	} DataToSave;

public:

	void OnUnitUpdateStats(ABaseUnit* Unit);

	void AddUnitDefeatedToResults(bool PlayerIsOwner);
	void AddMatchFinishedToResults(bool PlayerWon);
	void SavePlayerResults();

	// Set a reference to the unit playing in this turn and modify the HUD
	void SetActiveUnit(ABaseUnit* NewActiveUnit);

	// Process the HitResult received from the PlayerController
	void ProcessClick(FHitResult& HitResult);

	// Try to reverse the current state to Idle or unselected if possible
	void ReverseState(UUserWidget* CombatPlayerWidget);

	// Change the CurrentState
	void ChangeState(EUnitState NewState, int32 ActionPosition = -1);

	// Delegate declaration
	FOnUnitSelected OnUnitSelected;
	FOnStateChanged OnStateChanged;

	ABaseUnit* GetActiveUnit() const { return ActiveUnit; };
	ABaseUnit* GetSelectedUnit() const { return SelectedUnit; };
};
