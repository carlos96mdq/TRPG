// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MMPlayerState.generated.h"

// Forward declarations
class UDataTable;
class AUnitModel;

/**
 * 
 */
UCLASS()
class TRPG_API AMMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	void LoadPlayerMatchesRecord();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Array of units that are in the current map
	TArray<FName> MyUnits;

	// 3D model of the selected units to show the user
	AUnitModel* SelectedUnitModel;

	// Data Table references
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* UnitsTable;

public:
	// Set the Selected Unit Model
	void SetSelectedUnit(int32 UnitIndex);

	// Return a reference to the units Icon (2DTexture)
	UTexture2D* GetUnitIcon(int32 UnitIndex) const;

	// Return how many units are saved in MyUnits array
	int32 GetUnitsQuantity() { return MyUnits.Num(); }
};
