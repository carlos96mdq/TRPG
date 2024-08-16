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

USTRUCT(BlueprintType)
struct FDamageTypeModifiers : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Bug = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Dark = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Dragon = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Electric = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Fairy = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Fighting = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Fire = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Flying = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Ghost = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Grass = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Ground = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Normal = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Ice = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Posion = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Psychic = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Rock = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Steel = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Water = 1;
};

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

	TArray<TArray<float>> DamageTypeModifiers;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* DamageTypeModifiersTable;

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
	int32 GetUnitsQuantity() const { return UnitsArray.Num(); };
	float GetDamageTypeModifier(int32 DamageType, int32 DefenderType, int32 DefenderSubType) const;
};
