// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGGameStateBase.h"
#include "BaseUnit.h"
#include "NpcUnit.h"
#include "Terrain.h"
#include "NpcController.h"
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
	NpcController = GetWorld()->SpawnActor<ANpcController>();

	GetWorld()->GetFirstPlayerController()->GetPlayerState<ATRPGPlayerState>()->SetTerrain(Terrain);

	// Get Unit selected by Player in MainMenu
	FName SelectedUnitName = FName("Bulbasaur");	//TODO eliminar esto y dejar el de abajo
	//FName SelectedUnitName = GetGameInstance()->GetSubsystem<UTRPGGameInstanceSubsystem>()->GetSelectedUnitName();

	// Load the Damage Type Modifiers datatable in a local array
	check(DamageTypeModifiersTable);
	TArray<TArray<FString>> WARDTArray = DamageTypeModifiersTable->GetTableData();
	TArray<float> InitializationArray;
	InitializationArray.Init(1, (int32)EUnitType::MAX);
	DamageTypeModifiers.Init(InitializationArray, (int32)EUnitType::MAX);

	for (int32 Attacker = 1; Attacker < WARDTArray.Num(); Attacker++)
	{
		for (int32 Defender = 1; Defender < WARDTArray.Num(); Defender++)
		{
			DamageTypeModifiers[Attacker][Defender] = FCString::Atof(*(WARDTArray[Attacker][Defender]));
		}
	}

	// If exist, spawn selected unit, else, spawn a default unit
	int32 NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(
		BaseUnitClass,
		FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(200.0f, 0.0f, 20.0f))
		)
	);
	UnitsArray[NewUnitIndex]->Init(SelectedUnitName, true);
	UnitsArray[NewUnitIndex]->FinishSpawning(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(200.0f, 0.0f, 20.0f)));

	// TEST: Create 2 default units
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

	// TEST: Create a npc
	NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ANpcUnit>(
		NpcUnitClass,
		FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(1200.0f, 0.0f, 20.0f))
		)
	);
	UnitsArray[NewUnitIndex]->Init(TEXT("Rattata"), false);
	UnitsArray[NewUnitIndex]->FinishSpawning(FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(600.0f, 200.0f, 20.0f)));

	StartGame();
}

void ATRPGGameStateBase::StartGame()
{
	// Init random seed
	FMath::RandInit(FDateTime::Now().GetTicks());

	// DEPRECATED 
	// Sort the units array according to their velocity
	/*
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
	*/

	// The function TurnStarts is called in all units so their initial stats like armor can be set
	for (ABaseUnit* Unit : UnitsArray)
		Unit->TurnStarts();

	OnGameStarts.Broadcast();
	SetNextTurn(true);
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

	// Set the next Player. In this case, the turn rotates between the player and the NpcController.
	if (bIsPlayerTurn)
		bIsPlayerTurn = false;
	else
		bIsPlayerTurn = true;

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

	OnNewTurnStarts.Broadcast(bIsPlayerTurn);
}

TArray<FVector> ATRPGGameStateBase::GetAllUnitLocations()
{
	TArray<FVector> Locations;

	for (ABaseUnit* Unit : UnitsArray)
		Locations.Emplace(Unit->GetActorLocation());

	return Locations;
}

ABaseUnit* ATRPGGameStateBase::GetUnitByIndex(int32 Index)
{
	if (Index >= 0 && Index < UnitsArray.Num())
		return UnitsArray[Index];
	else
		return nullptr;
}

float ATRPGGameStateBase::GetDamageTypeModifier(int32 DamageType, int32 DefenderType, int32 DefenderSubType) const
{
	float ResistanceValue = DamageTypeModifiers[DamageType][DefenderType] * DamageTypeModifiers[DamageType][DefenderSubType];
	return ResistanceValue;
}