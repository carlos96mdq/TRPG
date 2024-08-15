// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terrain.generated.h"

// Forward declarations
class ABaseTile;
class ABaseUnit;

UCLASS()
class TRPG_API ATerrain : public AActor
{
	GENERATED_BODY()
	
	// Called to create a new map whith tiles
	void CreateMap();

	// Tiles quantity that map uses
	int32 MapMaxRows = 9;
	int32 MapMaxColumns = 9;
	float TileSize = 200.0f;	//TODO esto deberia obtenerlo desde una constante en game state para luego obtenerla en TODOS lados

	// Array of tiles that compound the current map
	TArray<ABaseTile*> TilesArray;
	
	// Array of tiles being selected at the moment
	TArray<ABaseTile*> AvailableTiles;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ATerrain();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Save in an array all the tiles that are available to the current action and show them in map
	void SetAvailableTiles(ABaseUnit* ActiveUnit, bool bShowTiles=true);

	// Check if the tile receive is inside the selected ones
	bool CheckAvailableTile(ABaseTile* TileNeeded);
	
	// Check if the tile receive is inside the selected ones
	bool CheckAvailableTile(FVector EnemyPosition);

	// Clean all the selected tiles in map
	void CleanAvailableTiles();

	// Get all the tiles that compund the path from the current position to the destination
	TArray<FVector> GetPath(ABaseTile* DestinationTile);

	int32 GetTileCost(FVector EnemyPosition) const;
};
