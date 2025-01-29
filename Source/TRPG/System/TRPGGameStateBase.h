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
class UTerrain;
class UUnitsManager;
class ANpcController;
enum class EUnitControllerOwner : uint8;

DECLARE_MULTICAST_DELEGATE(FOnGameStarts)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewTurnStarts, EUnitControllerOwner)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActiveUnitSet, ABaseUnit*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameOver, EUnitControllerOwner)

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

struct FUnitInitData
{
	FName Archetype;
	EUnitControllerOwner ControllerOwner;
	FTransform Transform;

	FUnitInitData(FName NewArchetype, EUnitControllerOwner NewControllerOwner, const FTransform& NewTransform)
		: Archetype(NewArchetype), ControllerOwner(NewControllerOwner), Transform(NewTransform) {}
};

/**
 *
 */
UCLASS()
class TRPG_API ATRPGGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

	// Tile class used to generate map
	UPROPERTY(EditAnywhere, Category = OwnAttributes)
	TSubclassOf<ABaseTile> BaseTileClass;
	UPROPERTY(EditAnywhere, Category = OwnAttributes)
	TSubclassOf<ABaseTile> WallTileClass;

	// Unit classes used to spawn units in world
	UPROPERTY(EditAnywhere, Category = OwnAttributes)
	TSubclassOf<ABaseUnit> BaseUnitClass;
	UPROPERTY(EditAnywhere, Category = OwnAttributes)
	TSubclassOf<ANpcUnit> NpcUnitClass;

	TArray<TArray<float>> DamageTypeModifiers;

	// Class Terrain that manage the map data
	UPROPERTY()
	TObjectPtr<UTerrain> Terrain;

	// Class that manages the units
	UPROPERTY()
	TObjectPtr<UUnitsManager> UnitsManager;

	// Class that controlles the actions and movements of all npcs in map
	TObjectPtr<ANpcController> NpcController;

	EUnitControllerOwner ControllerTurn;

	// Unit whose turn is the current one
	int32 ActiveUnitIndex = -1;

	UPROPERTY(EditAnywhere)
	UDataTable* DamageTypeModifiersTable;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:

	ATRPGGameStateBase();

	void StartGame();
	void SetNextTurn();
	void ControllerLostGame(EUnitControllerOwner ControllerOwner);

	// Fill a reference TArray with the current location of all units
	TArray<FVector> GetAllUnitLocations() const;

	FOnGameStarts OnGameStarts;
	FOnNewTurnStarts OnNewTurnStarts;
	FOnActiveUnitSet OnActiveUnitSet;
	FOnGameOver OnGameOver;

	TSubclassOf<ABaseTile> GetBaseTileClass() const { return BaseTileClass; };
	TSubclassOf<ABaseTile> GetWallTileClass() const { return WallTileClass; };
	TSubclassOf<ABaseUnit> GetBaseUnitClass() const { return BaseUnitClass; };
	TSubclassOf<ANpcUnit> GetNpcUnitClass() const { return NpcUnitClass; };
	UTerrain* GetTerrain() const { return Terrain; };
	ABaseUnit* GetUnitByIndex(int32 Index) const;
	int32 GetUnitsNum() const;
	float GetDamageTypeModifier(int32 DamageType, int32 DefenderType, int32 DefenderSubType) const;
};
