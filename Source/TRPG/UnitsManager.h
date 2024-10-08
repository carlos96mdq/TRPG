// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitsManager.generated.h"

class ABaseUnit;
struct FUnitInitData;
enum class EUnitControllerOwner : uint8;

UCLASS()
class TRPG_API AUnitsManager : public AActor
{
	GENERATED_BODY()
	
	// Array of units that are in the current map
	TArray<ABaseUnit*> UnitsArray;

	void OnUnitUpdateStats(int32 UnitIndex);
	void OnUnitStops(int32 UnitIndex);

public:	
	// Create new units, initialize them and save their reference in the UnitsArray
	void CreateNewUnits(const TArray<FUnitInitData>& UnitsData);
	void CreateNewUnit(FName Archetype, EUnitControllerOwner ControllerOwner, const FTransform& Location);

	// Manage units changes ath the start and end of the turn
	void UnitsStartTurn(EUnitControllerOwner ControllerOwner);
	void UnitsEndTurn(EUnitControllerOwner ControllerOwner);

	TArray<FVector> GetAllUnitLocations();
	ABaseUnit* GetUnitByIndex(int32 Index);
	int32 GetUnitsNum() const;
};
