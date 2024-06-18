// Fill out your copyright notice in the Description page of Project Settings.


#include "Terrain.h"
#include "TRPGGameStateBase.h"
#include "BaseTile.h"
#include "BaseUnit.h"

ATerrain::ATerrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATerrain::BeginPlay()
{
	Super::BeginPlay();
	
	CreateMap();
}

void ATerrain::CreateMap()
{
	if (ATRPGGameStateBase* GameState = Cast<ATRPGGameStateBase>(GetWorld()->GetGameState()))
	{
		TSubclassOf<ABaseTile> NormalTile = GameState->GetBaseTileClass();
		TSubclassOf<ABaseTile> WallTile = GameState->GetWallTileClass();

		for (auto i = 0; i < MapMaxRows; i++)
		{
			for (size_t j = 0; j < MapMaxColumns; j++)
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

void ATerrain::ShowAvailableTiles(ABaseUnit* ActiveUnit)
{
	// First clean any current available tile
	CleanAvailableTiles();
	
	FVector UnitLocation = ActiveUnit->GetActorLocation();
	int MinRange = 0;
	int MaxRange = 0;

	// Depending on action, set min and max ranges
	EUnitState State = ActiveUnit->GetUnitState();
	if (State == EUnitState::ReadyToMove)
	{
		MaxRange = ActiveUnit->GetMovement();
	}
	else if (State == EUnitState::ReadyToCombat)
	{
		MinRange = ActiveUnit->GetCombatActionRangeMin();
		MaxRange = ActiveUnit->GetCombatActionRangeMax();
	}

	// Get the current location of all units
	TArray<FVector> AllUnitLocations;
	Cast<ATRPGGameStateBase>(GetWorld()->GetGameState())->GetAllUnitLocations(AllUnitLocations);

	// Get all tiles meet the max range requirement and are no obstacle tiles (walls tiles e.g.)
	// Save all obstacle tiles to use when attacking
	// Get the adjacent tiles too
	TArray<ABaseTile*> PossibleTiles;
	TArray<ABaseTile*> ObstacleTiles;
	for (ABaseTile* Tile : TilesArray)
	{
		// Get tile distance from unit
		FVector TileLocation = Tile->GetActorLocation();
		float TileDistance = (FMath::Abs(UnitLocation.X - TileLocation.X) + FMath::Abs(UnitLocation.Y - TileLocation.Y)) / TileSize;

		// The tile is inside the max range
		if (TileDistance <= MaxRange + 0.1f)
		{
			bool bAvailableTile = true;

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
				if (Tile->GetType() == TileType::Wall)
				{
					ObstacleTiles.Emplace(Tile);
				}
				// If it is an adjacent tile it is saved as available (as they are needed to calculate the pathfinding the min range is not taken into account yet)
				else if (TileDistance <= 1.1f)
				{
					if (TileDistance <= 0.9f)
					{
						Tile->SetMovementCost(0);
					}
					else
					{
						Tile->SetMovementCost(1);
					}
					AvailableTiles.Emplace(Tile);
				}
				else
				{
					PossibleTiles.Emplace(Tile);
				}
			}
		}
	}

	// Get all the available tiles to this action
	for (int32 i = 2; i <= MaxRange; i++)
	{
		// Check for all adjacent tiles to the already selected ones
		for (ABaseTile* ReferenceTile : AvailableTiles)
		{
			// Only check in the further tiles from the center
			if (ReferenceTile->GetMovementCost() == i - 1)
			{
				FVector ReferenceTileLocation = ReferenceTile->GetActorLocation();

				// Get all the adjacent tiles to the currently selected tiles
				for (ABaseTile* PossibleTile : PossibleTiles)
				{
					// Check if tile is not already inside de SelectedTiles table
					if (PossibleTile->GetMovementCost() == -1)
					{
						FVector PossibleTileLocation = PossibleTile->GetActorLocation();
						float TileDistance = (FMath::Abs(ReferenceTileLocation.X - PossibleTileLocation.X) + FMath::Abs(ReferenceTileLocation.Y - PossibleTileLocation.Y)) / TileSize;

						if (TileDistance <= (1.1f))
						{
							PossibleTile->SetMovementCost(i);
						}
					}
				}
			}
		}

		// Insert all posible tiles to the SelectedTiles table
		for (ABaseTile* PossibleTile : PossibleTiles)
		{
			if (PossibleTile->GetMovementCost() == i)
			{
				AvailableTiles.Emplace(PossibleTile);
			}
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Then, %d tiles were catagorized as available"), AvailableTiles.Num());

	// Delete all tiles limited by the min range
	// Or when attacking, delete all tiles that can't be reached because of an obstacle or a min range
	if (MinRange >= 1 || ActiveUnit->GetUnitState() == EUnitState::ReadyToCombat)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Now, all the tiles that are blocked or are more near than the minrange of %d will be removed"), MinRange);
		TArray<int32> IndexesToRemove;

		// Verify in each obstacle all tiles that are view-blocked
		for (int32 i=AvailableTiles.Num()-1; i>=0; --i)
		{
			ABaseTile* AvailableTile = AvailableTiles[i];
			FVector Destination = AvailableTile->GetActorLocation();
			Destination.Z = UnitLocation.Z;
			FHitResult HitObstacle;

			// Check with a raycast if an obstacle is in between the unit and the destination (in terms of visibility)
			if (AvailableTile->GetMovementCost() < MinRange
				|| GetWorld()->SweepSingleByChannel(HitObstacle, UnitLocation, Destination, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(1.0f)))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Remove this tile because its Movement Cost %d is less than the MinRange %d will be removed"), AvailableTile->GetMovementCost(), MinRange);
				//DrawDebugCylinder(GetWorld(), UnitLocation, Destination, 1.0f, 5, FColor::Red, true, 10.0f);
				AvailableTile->SetMovementCost(-1);
				AvailableTiles.RemoveAtSwap(i);
			}
			else
			{
				//DrawDebugCylinder(GetWorld(), UnitLocation, Destination, 1.0f, 5, FColor::Green, true, 10.0f);
			}
		}
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Finally, %d tiles were catagorized as available"), AvailableTiles.Num());

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

bool ATerrain::CheckAvailableTile(ABaseTile* TileNeeded)
{
	auto TileIdx = AvailableTiles.Find(TileNeeded);

	if (TileIdx == INDEX_NONE)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Tile no encontrado"));
		return false;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Tile encontrado"));
		return true;
	}
}

bool ATerrain::CheckAvailableTile(FVector EnemyPosition)
{
	//UE_LOG(LogTemp, Warning, TEXT("Checking if the unit is in an available terrain inside a list of %d tiles"), AvailableTiles.Num());
	for (auto i = 0; i < AvailableTiles.Num(); i++)
	{
		FVector EnemyTilePosition = FVector(EnemyPosition.X, EnemyPosition.Y, 0.0f);
		if (AvailableTiles[i]->GetActorLocation() == EnemyTilePosition)
		{
			//UE_LOG(LogTemp, Warning, TEXT("It is in an available terrain"))
			return true;
		}
	}
	return false;
}

void ATerrain::CleanAvailableTiles()
{
	for (auto Tile : AvailableTiles)
	{
		Tile->SetGlowingEffect(false);
		Tile->SetMovementCost(-1);
	}
	AvailableTiles.Empty();
}

TArray<FVector> ATerrain::GetPath(ABaseTile* DestinationTile)
{
	ABaseTile* CurrentTile = DestinationTile;
	FVector CurrentTileLocation = CurrentTile->GetActorLocation();
	TArray<FVector> PathLocations;

	// Add Destination as the last location
	PathLocations.Emplace(FVector(CurrentTileLocation.X, CurrentTileLocation.Y, CurrentTileLocation.Z));

	// Check for each tile location in the path
	// As all movement cost are already measured, this algorithm starts looking from the destination to the origin
	for (int32 i = (DestinationTile->GetMovementCost() - 1); i > 0; i--)
	{
		TArray<ABaseTile*> AdjacentTiles;

		// Get all available adjacent tiles to the current tile
		for (ABaseTile* AvailableTile : AvailableTiles)
		{
			// Get tile distance from unit
			FVector AvailableTileLocation = AvailableTile->GetActorLocation();
			float TileDistance = FMath::Abs(CurrentTileLocation.X - AvailableTileLocation.X) + FMath::Abs(CurrentTileLocation.Y - AvailableTileLocation.Y);

			// Get all adjacent tiles
			if (TileDistance / TileSize <= 1.1f && TileDistance / TileSize > 0.9f)
			{
				AdjacentTiles.Emplace(AvailableTile);
			}
		}

		// Get first available adjacent tile to meet the movement cost requeriment
		for (ABaseTile* AdjacentTile : AdjacentTiles)
		{
			if (AdjacentTile->GetMovementCost() == i)
			{
				// Set new current tile and save its location in the path
				CurrentTile = AdjacentTile;
				CurrentTileLocation = CurrentTile->GetActorLocation();
				PathLocations.Emplace(FVector(CurrentTileLocation.X, CurrentTileLocation.Y, CurrentTileLocation.Z));
				break;
			}
		}
	}
	return PathLocations;
}

void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

