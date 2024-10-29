// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain.generated.h"

// Forward declarations
class ABaseTile;
class ABaseUnit;

UCLASS()
class TRPG_API UTerrain : public UObject
{
	GENERATED_BODY()
	
	// Tiles quantity that map uses
	int32 MapMaxRows = 9;
	int32 MapMaxColumns = 9;
	float TileSize = 200.0f;	//TODO esto deberia obtenerlo desde una constante en game state para luego obtenerla en TODOS lados

	// Array of tiles that compound the current map
	TArray<ABaseTile*> TilesArray;
	
	// Array of tiles being selected at the moment
	TArray<ABaseTile*> AvailableTiles;

	// Set the movement cost of all tiles from the Origin (0 cost) to the max range. In case of not range, set the movement cost of all tiles
	// The obstacle tiles as walls are never processed
	TArray<ABaseTile*> SetMovementCost(ABaseTile* OriginTile, int32 MaxRange=-1, int32 MinRange=0);

	// Clean all Tiles send to this function
	void CleanTiles(const TArray<ABaseTile*>& TilesToClean);

public:	
	// Called to create a new map whith tiles
	void CreateMap();

	ABaseUnit* FindNearestEnemy(ABaseUnit* ActiveUnit);

	// Save in an array all the tiles that are available to the current action and show them in map
	void SetAvailableTiles(ABaseUnit* ActiveUnit, bool bShowTiles=true);

	// Check if the tile receive is inside the selected ones
	bool CheckAvailableTile(ABaseTile* TileNeeded);
	
	// Check if the tile receive is inside the selected ones
	bool CheckAvailableTile(FVector EnemyPosition);

	// Clean all the available tiles
	void CleanAvailableTiles();

	// Get all the tiles in order to make a path between the origin and a destination, being the first tile the destination.
	// In case a destination further away than the max units range according to its current energy, the further possible path will be returned
	// With the Destination OffSet, the function caller indicates that the Path has to reach the destination - offset tiles
	TArray<FVector> GetPath(ABaseTile* DestinationTile);
	TArray<FVector> GetPath(const FVector& Destination, int32 DestinationOffSet=0);

	int32 GetTileCost(FVector EnemyPosition) const;
};
