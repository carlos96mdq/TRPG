// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Terrain.h"
#include "System/TRPGGameStateBase.h"
#include "Units/BaseTile.h"
#include "Units/BaseUnit.h"

DEFINE_LOG_CATEGORY_STATIC(LogTerrain, Log, All)

void UTerrain::CreateMap()
{
	if (ATRPGGameStateBase* GameState = Cast<ATRPGGameStateBase>(GetWorld()->GetGameState()))
	{
		TSubclassOf<ABaseTile> NormalTile = GameState->GetBaseTileClass();
		TSubclassOf<ABaseTile> WallTile = GameState->GetWallTileClass();

		for (auto i = 0; i < MapMaxRows; i++)
		{
			for (int32 j = 0; j < MapMaxColumns; j++)
			{
				TSubclassOf<ABaseTile> UsedTile;
				
				if (j > 5 && j < 7 && i > 2 && i < 30 || j > 2 && j < 4 && i > 5 && i < 7)
				{
					UsedTile = WallTile;
				}
				else
				{
					UsedTile = NormalTile;
				}

				TilesArray.Emplace(GetWorld()->SpawnActor<ABaseTile>(
					UsedTile,
					FVector(TileSize * j, TileSize * i, 0.0f),
					FRotator(0.0f, 0.0f, 0.0f)
					)
				);
			}
		}
	}
	else
	{
		checkNoEntry();
	}
}


TArray<ABaseTile*> UTerrain::SetMovementCost(ABaseTile* OriginTile, int32 MaxRange, int32 MinRange)
{
	if (OriginTile == nullptr)
	{
		UE_LOG(LogTerrain, Warning, TEXT("Couldn't set the tiles movement cost because none OriginTile was received"));
		return TArray<ABaseTile*>();
	}

	OriginTile->SetMovementCost(0);
	TArray<ABaseTile*> ProcessedTiles = { OriginTile };
	TArray<ABaseTile*> TilesInRange;

	if (MinRange == 0)
	{
		TilesInRange.Emplace(OriginTile);
	}
	
	// Iterate through all tiles to set their cost, but only save as available the ones inside the MaxRange
	bool bThereAreMoreTilesToProcess = true;
	int32 Range = 1;

	while (bThereAreMoreTilesToProcess)
	{
		// In each Range iteration we check for the adjacent tiles to the furthers of the already processed ones
		for (ABaseTile* ReferenceTile : ProcessedTiles)
		{
			if (ReferenceTile->GetType() == TileType::Wall)
			{
				continue;
			}

			if (ReferenceTile->GetMovementCost() == Range - 1)
			{
				FVector ReferenceTileLocation = ReferenceTile->GetActorLocation();

				// Get all the adjacent tiles to the referenced one
				for (ABaseTile* TileToProcess : TilesArray)
				{
					// Check if tile is not already processed
					if (TileToProcess->GetMovementCost() == -1)
					{
						FVector TileToProcessLocation = TileToProcess->GetActorLocation();
						float TileDistance = (FMath::Abs(ReferenceTileLocation.X - TileToProcessLocation.X) + FMath::Abs(ReferenceTileLocation.Y - TileToProcessLocation.Y)) / TileSize;

						if (TileDistance <= (1.1f))
						{
							TileToProcess->SetMovementCost(Range);
						}
					}
				}
			}
		}

		// Now insert all the processed tiles inside the return array only if it is into the range. In case no range was specified, return all tiles.
		// Always assume that there are not more tiles to process until we find at least one in this iteration. In the final iteration none tile will be processed
		bThereAreMoreTilesToProcess = false;
		for (ABaseTile* Tile : TilesArray)
		{
			if (Tile->GetMovementCost() == Range)
			{
				bThereAreMoreTilesToProcess = true;
				ProcessedTiles.Emplace(Tile);
				
				if (MaxRange == -1 || Range <= MaxRange && Range >= MinRange)
				{
					TilesInRange.Emplace(Tile);
				}
			}
		}

		Range++;
	}
	return TilesInRange;
}

ABaseUnit* UTerrain::FindNearestEnemy(ABaseUnit* ActiveUnit)
{
	CleanTiles(TilesArray);

	// Set the cost of all tiles, taking as origin the one below this unit
	FVector UnitLocation = ActiveUnit->GetActorLocation();
	ABaseTile* ActiveUnitTile = nullptr;

	for (ABaseTile* Tile : TilesArray)
	{
		// Get tile distance from unit
		FVector TileLocation = Tile->GetActorLocation();
		float TileDistance = (FMath::Abs(UnitLocation.X - TileLocation.X) + FMath::Abs(UnitLocation.Y - TileLocation.Y)) / TileSize;

		// The tile is inside the max range
		if (TileDistance <= 0.9f)
		{
			ActiveUnitTile = Tile;
			break;
		}
	}

	if (ActiveUnitTile)
	{
		SetMovementCost(ActiveUnitTile);

		ABaseUnit* NearestEnemy = nullptr;
		int32 NearestEnemyCost = -1;

		ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

		for (int32 UnitIdx = 0; UnitIdx < GameState->GetUnitsNum(); UnitIdx++)
		{
			ABaseUnit* EnemyUnit = GameState->GetUnitByIndex(UnitIdx);

			if (EnemyUnit && EnemyUnit->GetControllerOwner() != ActiveUnit->GetControllerOwner())
			{
				FVector EnemyUnitLocation = EnemyUnit->GetActorLocation();
				ABaseTile* EnemyUnitTile = nullptr;

				for (ABaseTile* Tile : TilesArray)
				{
					FVector TileLocation = Tile->GetActorLocation();
					float TileDistance = (FMath::Abs(UnitLocation.X - TileLocation.X) + FMath::Abs(UnitLocation.Y - TileLocation.Y)) / TileSize;

					// The tile is inside the max range
					if (TileDistance <= 0.9f)
					{
						EnemyUnitTile = Tile;
						break;
					}
				}

				if (EnemyUnitTile && (NearestEnemyCost == -1 || EnemyUnitTile->GetMovementCost() < NearestEnemyCost))
				{
					NearestEnemy = EnemyUnit;
					NearestEnemyCost = EnemyUnitTile->GetMovementCost();
				}
			}
		}
		return NearestEnemy;
	}
	else
	{
		UE_LOG(LogTerrain, Warning, TEXT("Couldn't find the tile below this unit when trying to find its nearest enemy"));
		return nullptr;
	}
}

void UTerrain::SetAvailableTiles(ABaseUnit* ActiveUnit, bool bShowTiles)
{
	// First clean any current available tile
	CleanTiles(TilesArray);
	
	FVector UnitLocation = ActiveUnit->GetActorLocation();
	int32 MinRange = 0;
	int32 MaxRange = 0;

	// Depending on action, set min and max ranges
	EUnitState State = ActiveUnit->GetUnitState();
	if (State == EUnitState::ReadyToMove)
	{
		MinRange = 1;
		MaxRange = ActiveUnit->GetEnergy();
	}
	else if (State == EUnitState::ReadyToCombat)
	{
		MinRange = ActiveUnit->GetCombatActionRangeMin();
		MaxRange = ActiveUnit->GetCombatActionRangeMax();
	}
	
	// Get the current location of all units
	TArray<FVector> AllUnitLocations = Cast<ATRPGGameStateBase>(GetWorld()->GetGameState())->GetAllUnitLocations();

	// Get all tiles meet the max range requirement and are no obstacle tiles (walls tiles e.g.)
	// Save all obstacle tiles to use when attacking
	// Get the adjacent tiles too
	TArray<ABaseTile*> PossibleTiles;
	ABaseTile* OriginTile = nullptr;
	for (ABaseTile* Tile : TilesArray)
	{
		// Get tile distance from unit
		FVector TileLocation = Tile->GetActorLocation();
		float TileDistance = (FMath::Abs(UnitLocation.X - TileLocation.X) + FMath::Abs(UnitLocation.Y - TileLocation.Y)) / TileSize;

		// The tile is inside the max range
		if (TileDistance <= MaxRange + 0.1f)
		{
			bool bAvailableTile = true;

			// Before anything save the origin tile. This is made here because later the oriign tile is excepted if in movement
			if (TileDistance <= 0.9f)
			{
				OriginTile = Tile;
			}
			// If the action is movement, discard all tiles where there are units
			if (ActiveUnit->GetUnitState() == EUnitState::ReadyToMove)
			{
				for (FVector RestrictedLocation : AllUnitLocations)
				{
					if ((TileLocation.X == RestrictedLocation.X && TileLocation.Y == RestrictedLocation.Y))
					{
						bAvailableTile = false;
						break;
					}
				}
			}
			if (bAvailableTile)
			{
				// Check if tile is an obstacle to save in other array
				if (Tile->GetType() != TileType::Wall)
				{
					PossibleTiles.Emplace(Tile);
				}

			}
		}
	}

	// Although we only ask and get the available tiles, we have to set the cost in all tiles
	AvailableTiles = SetMovementCost(OriginTile, MaxRange, MinRange);

	// Delete all tiles that can't be reached because of an obstacle (this doesn't affect the movement, only attacks)
	if (ActiveUnit->GetUnitState() == EUnitState::ReadyToCombat)
	{
		TArray<int32> IndexesToRemove;

		// Verify in each obstacle all tiles that are view-blocked
		for (int32 i=AvailableTiles.Num()-1; i>=0; --i)
		{
			ABaseTile* AvailableTile = AvailableTiles[i];
			FVector Destination = AvailableTile->GetActorLocation();
			Destination.Z = UnitLocation.Z;
			FHitResult HitObstacle;

			if (GetWorld()->SweepSingleByChannel(HitObstacle, UnitLocation, Destination, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(1.0f)))
			{
				//DrawDebugCylinder(GetWorld(), UnitLocation, Destination, 1.0f, 5, FColor::Red, true, 10.0f);
				AvailableTiles.RemoveAtSwap(i);
			}
			else
			{
				//DrawDebugCylinder(GetWorld(), UnitLocation, Destination, 1.0f, 5, FColor::Green, true, 10.0f);
			}
		}
	}
	
	if (bShowTiles)
	{
		int32 EffectIdx = 0;
		switch (ActiveUnit->GetUnitState())
		{
		case EUnitState::ReadyToMove:
			EffectIdx = 1;
			break;
		case EUnitState::ReadyToCombat:
			EffectIdx = 2;
			break;
		default:
			break;
		}

		// Show in map the selected tiles
		for (ABaseTile* Tile : AvailableTiles)
		{
			//float PosX = Tile->GetActorLocation().X;
			//float PosY = Tile->GetActorLocation().Y;
			//UE_LOG(LogTemp, Warning, TEXT("Posición del Tile seleccionado: x:%f, y:%f"), PosX, PosY);
			Tile->SetGlowingEffect(true, EffectIdx);
		}
	}
}

bool UTerrain::CheckAvailableTile(ABaseTile* TileNeeded)
{
	int32 TileIdx = AvailableTiles.Find(TileNeeded);

	if (TileIdx == INDEX_NONE)
		return false;
	else
		return true;
}

bool UTerrain::CheckAvailableTile(const FVector& TileLocation)
{
	for (auto i = 0; i < AvailableTiles.Num(); i++)
	{
		FVector EnemyTilePosition = FVector(TileLocation.X, TileLocation.Y, 0.0f);
		if (AvailableTiles[i]->GetActorLocation() == EnemyTilePosition)
			return true;
	}
	return false;
}

void UTerrain::CleanAvailableTiles()
{
	CleanTiles(AvailableTiles);
	AvailableTiles.Empty();
}

void UTerrain::CleanTiles(const TArray<ABaseTile*>& TilesToClean)
{
	for (auto Tile : TilesToClean)
	{
		Tile->SetGlowingEffect(false);
		Tile->SetMovementCost(-1);
	}
}

TArray<FVector> UTerrain::GetPath(ABaseTile* DestinationTile)
{
	FVector TileLocation = DestinationTile->GetActorLocation();
	return GetPath(TileLocation);
}

TArray<FVector> UTerrain::GetPath(const FVector& Destination, int32 DestinationOffSet)
{
	FVector CurrentTileLocation(Destination);
	TArray<FVector> PathLocations;
	int32 LeftOffset = DestinationOffSet;

	// Add Destination as the last location if reachable with the unit current energy
	if (LeftOffset <= 0 && CheckAvailableTile(CurrentTileLocation))
	{
		PathLocations.Emplace(FVector(CurrentTileLocation.X, CurrentTileLocation.Y, CurrentTileLocation.Z));
	}
	else
	{
		LeftOffset--;
	}

	// Check for each tile location in the path
	// As all movement cost are already measured, this algorithm starts looking from the destination to the origin
	for (int32 i = GetTileCost(Destination) - 1; i > 0; i--)
	{
		TArray<ABaseTile*> AdjacentTiles;

		// Get all available adjacent tiles to the current tile
		for (ABaseTile* PossibleAdjacentTile : TilesArray)
		{
			// Get tile distance from unit
			FVector PossibleAdjacentTileLocation = PossibleAdjacentTile->GetActorLocation();
			float TileDistance = FMath::Abs(CurrentTileLocation.X - PossibleAdjacentTileLocation.X) + FMath::Abs(CurrentTileLocation.Y - PossibleAdjacentTileLocation.Y);

			// Get all adjacent tiles
			if (TileDistance / TileSize <= 1.1f && TileDistance / TileSize > 0.9f)
			{
				AdjacentTiles.Emplace(PossibleAdjacentTile);
			}
		}

		// Get first available adjacent tile to meet the movement cost requeriment. Only 1 tile should be retreive in this section
		for (ABaseTile* AdjacentTile : AdjacentTiles)
		{
			if (AdjacentTile->GetMovementCost() == i)
			{
				CurrentTileLocation = AdjacentTile->GetActorLocation();

				// Save tile inside the path ONLY if it is an available tile
				if (LeftOffset <= 0 && CheckAvailableTile(AdjacentTile))
				{
					PathLocations.Emplace(FVector(CurrentTileLocation.X, CurrentTileLocation.Y, CurrentTileLocation.Z));
				}
				else
				{
					LeftOffset--;
				}

				break;
			}
		}
	}
	return PathLocations;
}

int32 UTerrain::GetTileCost(FVector EnemyPosition) const
{
	for (auto i = 0; i < TilesArray.Num(); i++)
	{
		FVector EnemyTilePosition = FVector(EnemyPosition.X, EnemyPosition.Y, 0.0f);
		if (TilesArray[i]->GetActorLocation() == EnemyTilePosition)
		{
			return TilesArray[i]->GetMovementCost();
		}
	}
	return -1;
}

