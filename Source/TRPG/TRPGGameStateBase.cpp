// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGGameStateBase.h"
#include "BaseUnit.h"
#include "Terrain.h"
#include "TRPGPlayerState.h"
#include "TRPGGameInstanceSubsystem.h"

// Called when the game starts or when spawned
void ATRPGGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	Terrain = GetWorld()->SpawnActor<ATerrain>();

	GetWorld()->GetFirstPlayerController()->GetPlayerState<ATRPGPlayerState>()->SetTerrain(Terrain);

	// Get Unit selected by Player in MainMenu
	FName SelectedUnitName = FName("Bulbasaur");	//TODO eliminar esto y dejar el de abajo
	//FName SelectedUnitName = GetGameInstance()->GetSubsystem<UTRPGGameInstanceSubsystem>()->GetSelectedUnitName();

	// If exist, spawn selected unit, else, spawn a default unit
	int32 NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(
		BaseUnitClass,
		FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(200.0f, 0.0f, 20.0f))
		)
	);
	UnitsArray[NewUnitIndex]->Init(SelectedUnitName);
	UnitsArray[NewUnitIndex]->FinishSpawning(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(200.0f, 0.0f, 20.0f)));

	// Create 2 default units to test
	NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(
		BaseUnitClass,
		FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(1200.0f, 0.0f, 20.0f))
		)
	);
	UnitsArray[NewUnitIndex]->Init(TEXT("Charmander"));
	UnitsArray[NewUnitIndex]->FinishSpawning(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(1200.0f, 0.0f, 20.0f)));

	NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(
		BaseUnitClass,
		FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(800.0f, 1600.0f, 50.0f))
		)
	);
	UnitsArray[NewUnitIndex]->Init(TEXT("Squirtle"));
	UnitsArray[NewUnitIndex]->FinishSpawning(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(800.0f, 1600.0f, 20.0f)));

	StartGame();
}

void ATRPGGameStateBase::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("StartGame in time: %d"), FDateTime::Now().GetTicks());
	// Init random seed
	FMath::RandInit(FDateTime::Now().GetTicks());

	// Sort the units array according to their velocity
	UnitsArray.Sort([](const ABaseUnit& LeftUnit, const ABaseUnit& RightUnit)
		{
			if (LeftUnit.GetUnitStat(EUnitStats::Velocity) > RightUnit.GetUnitStat(EUnitStats::Velocity))
				return true;
			else if (LeftUnit.GetUnitStat(EUnitStats::Velocity) == RightUnit.GetUnitStat(EUnitStats::Velocity))
			{
				// If 2 units has the same velocity, they ar sorted randomly
				float RandomValue = FMath::FRand();
				if (RandomValue > 0.5f)
					return true;
				else
					return false;
			}
			else
				return false;
		}
	);

	// Set first turn
	SetNextTurn();
}

void ATRPGGameStateBase::SetNextTurn()
{
	// First, if there was an Active Unit, make its turn end check
	if (ActiveUnitIndex >= 0)
	{
		check(UnitsArray[ActiveUnitIndex]);
		UnitsArray[ActiveUnitIndex]->TurnEnds();
	}

	// Continue to the next unit
	ActiveUnitIndex++;
	if (ActiveUnitIndex >= UnitsArray.Num())
		ActiveUnitIndex = 0;
	check(UnitsArray[ActiveUnitIndex]);

	UE_LOG(LogTemp, Warning, TEXT("SetNextTurn ActiveUnitIndex: %d"), ActiveUnitIndex);
	
	// Make this current unit turn starts check
	UnitsArray[ActiveUnitIndex]->TurnStarts();

	// Send delegate sign
	OnActiveUnitSet.Broadcast(UnitsArray[ActiveUnitIndex]);
}

void ATRPGGameStateBase::GetAllUnitLocations(TArray<FVector>& Locations)
{
	for (ABaseUnit* Unit : UnitsArray)
	{
		Locations.Emplace(Unit->GetActorLocation());
	}
}

ABaseUnit* ATRPGGameStateBase::GetActiveUnit()
{
	check(UnitsArray[ActiveUnitIndex]);
	return UnitsArray[ActiveUnitIndex];
}

ABaseUnit* ATRPGGameStateBase::GetUnitByActiveUnitReference(int32 offset)
{
	// Get unit index
	int32 UnitIndex = ActiveUnitIndex + offset;
	
	// Check if the index is insedi the array, else get a valid index 
	if (UnitIndex >= UnitsArray.Num())
	{
		int32 OverloadQuantity = FMath::Floor(UnitIndex / UnitsArray.Num());
		UnitIndex = UnitIndex - UnitsArray.Num() * OverloadQuantity;
	}
	else if (UnitIndex < 0)
	{
		int32 OverloadQuantity = FMath::Floor(FMath::Abs(UnitIndex) / UnitsArray.Num());
		UnitIndex = UnitIndex + UnitsArray.Num() * (OverloadQuantity + 1);
	}
	
	return UnitsArray[UnitIndex];
}