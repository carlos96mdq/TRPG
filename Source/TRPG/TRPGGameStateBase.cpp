// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGGameStateBase.h"
#include "BaseUnit.h"
#include "Terrain.h"
#include "TRPGPlayerState.h"
#include "TRPGGameInstanceSubsystem.h"

ATRPGGameStateBase::ATRPGGameStateBase()
{
}

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
	UnitsArray[NewUnitIndex]->Init(SelectedUnitName, true);
	UnitsArray[NewUnitIndex]->FinishSpawning(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(200.0f, 0.0f, 20.0f)));

	// Create 2 default units to test
	NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(
		BaseUnitClass,
		FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(1200.0f, 0.0f, 20.0f))
		)
	);
	UnitsArray[NewUnitIndex]->Init(TEXT("Charmander"), true);
	UnitsArray[NewUnitIndex]->FinishSpawning(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(1200.0f, 0.0f, 20.0f)));

	NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(
		BaseUnitClass,
		FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(800.0f, 1600.0f, 50.0f))
		)
	);
	UnitsArray[NewUnitIndex]->Init(TEXT("Squirtle"), true);
	UnitsArray[NewUnitIndex]->FinishSpawning(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(800.0f, 1600.0f, 20.0f)));

	StartGame();
}

void ATRPGGameStateBase::StartGame()
{
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

	// Check for all added units. Then if they are player's units, add an id to identify the unit.
	// Then call the function to StartTurn in ALL units so the initial armor is set to everybody
	uint8 UnitId = 1;
	for (ABaseUnit* Unit : UnitsArray)
	{
		check(Unit);
		Unit->TurnStarts();
	}

	// Set first turn
	SetNextTurn(true);

	// Notify everybody that the game has starts
	OnGameStarts.Broadcast();
}

void ATRPGGameStateBase::SetNextTurn(bool bFirstTurn)
{
	if (!bFirstTurn)
	{
		for (ABaseUnit* Unit : UnitsArray)
		{
			check(Unit);
			if (Unit->IsPlayer() == bIsPlayerTurn)
			{
				Unit->TurnEnds();
			}
		}
	}

	// First, if there was an Active Unit, make its turn end check
	//if (ActiveUnitIndex >= 0)
	//{
	//	check(UnitsArray[ActiveUnitIndex]);
	//	UnitsArray[ActiveUnitIndex]->TurnEnds();
	//}

	// Set the next Player. In this case, the turn rotates between the player and the npc
	if (bIsPlayerTurn)
		bIsPlayerTurn = true;	//TODO POR AHORA ESTO ESTA EN TRUE PARA FORZAR UN LOOP CONTINUO DEL PLAYER. PARA IMPLEMENTAR LA AI, CAMBIAR POR FALSE
	else
		bIsPlayerTurn = true;

	// Continue to the next unit
	//ActiveUnitIndex++;
	//if (ActiveUnitIndex >= UnitsArray.Num())
	//	ActiveUnitIndex = 0;
	//check(UnitsArray[ActiveUnitIndex]);

	if (!bFirstTurn)
	{
		for (ABaseUnit* Unit : UnitsArray)
		{
			check(Unit);
			if (Unit->IsPlayer() == bIsPlayerTurn)
			{
				Unit->TurnStarts();
			}
		}
	}

	// Make this current unit turn starts check
	//UnitsArray[ActiveUnitIndex]->TurnStarts();

	// Notify everybody that a new turn has started
	OnNewTurnStarts.Broadcast(bIsPlayerTurn);
	//OnActiveUnitSet.Broadcast(UnitsArray[ActiveUnitIndex]);
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

ABaseUnit* ATRPGGameStateBase::GetUnitByIndex(int32 Index)
{
	if (Index >= 0 && Index < UnitsArray.Num())
		return UnitsArray[Index];
	else
		return nullptr;
}