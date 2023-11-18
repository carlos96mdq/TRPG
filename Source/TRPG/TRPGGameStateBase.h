// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TRPGGameStateBase.generated.h"

// Forward declarations
class ABaseTile;
class ABaseUnit;
class ATerrain;

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

	// Base Unit class used to spawn units in world
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = OwnAttributes, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseUnit> BaseUnitClass;

	// Class Terrain that manage the map data
	ATerrain* Terrain;
	
	// Array of units that are in the current map
	TArray<ABaseUnit*> UnitsArray;

	// Unit whose turn is the current one
	//ABaseUnit* ActiveUnit;
	int32 ActiveUnitIndex = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Initialize the turn-system
	void StartGame();
	
	// Set next turn
	void SetNextTurn();

	// Fill a reference TArray with the current location of all units
	void GetAllUnitLocations(TArray<FVector> &Locations);

	FOnActiveUnitSet OnActiveUnitSet;

	const TSubclassOf<ABaseTile> GetBaseTileClass() const { return BaseTileClass; };
	const TSubclassOf<ABaseTile> GetWallTileClass() const { return WallTileClass; };
	ATerrain* GetTerrain() const { return Terrain; };
	ABaseUnit* GetActiveUnit();
	// In this case the BaseUnit returned is the one with the offset with the ActiveUnit in the UnitsArray 
	ABaseUnit* GetUnitByActiveUnitReference(int32 offset);
};
