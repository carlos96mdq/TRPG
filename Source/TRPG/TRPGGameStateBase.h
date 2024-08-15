// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Engine/DataTable.h"
#include "TRPGGameStateBase.generated.h"

// Forward declarations
class ABaseTile;
class ABaseUnit;
class ANpcUnit;
class ATerrain;
class ANpcController;

DECLARE_MULTICAST_DELEGATE(FOnGameStarts)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewTurnStarts, bool)	// True states that its the Player's turn, otherwise is an NPC turn
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActiveUnitSet, ABaseUnit*)

/**
 * 
 */
UCLASS()
class TRPG_API ATRPGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
	// Base Tile class used to generate map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OwnAttributes, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseTile> BaseTileClass;
	
	// Wall Tile class used to generate map
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OwnAttributes, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseTile> WallTileClass;

	// Unit classes used to spawn units in world
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OwnAttributes, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseUnit> BaseUnitClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OwnAttributes, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ANpcUnit> NpcUnitClass;

	// Class Terrain that manage the map data
	ATerrain* Terrain;

	// Class that controlles the actions and movements of all npcs in map
	ANpcController* NpcController;
	
	// Array of units that are in the current map
	TArray<ABaseUnit*> UnitsArray;

	bool bIsPlayerTurn = false;	// As the game is an one player game, this var defines if its the players turn or the npc turn

	// Unit whose turn is the current one
	int32 ActiveUnitIndex = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	ATRPGGameStateBase();

	// Initialize the turn-system
	void StartGame();
	
	// Set next turn
	void SetNextTurn(bool bFirstTurn=false);

	// Fill a reference TArray with the current location of all units
	void GetAllUnitLocations(TArray<FVector> &Locations);

	FOnGameStarts OnGameStarts;
	FOnNewTurnStarts OnNewTurnStarts;
	FOnActiveUnitSet OnActiveUnitSet;

	const TSubclassOf<ABaseTile> GetBaseTileClass() const { return BaseTileClass; };
	const TSubclassOf<ABaseTile> GetWallTileClass() const { return WallTileClass; };
	ATerrain* GetTerrain() const { return Terrain; };
	ABaseUnit* GetActiveUnit();
	// In this case the BaseUnit returned is the one with the offset with the ActiveUnit in the UnitsArray 
	ABaseUnit* GetUnitByActiveUnitReference(int32 offset);
	ABaseUnit* GetUnitByIndex(int32 Index);
	const int32 GetUnitsQuantity() const { return UnitsArray.Num(); };
};
