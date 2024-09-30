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
	//FName SelectedUnitName = FName("Bulbasaur");	//TODO eliminar esto y dejar el de abajo
	FName SelectedUnitName = GetGameInstance()->GetSubsystem<UTRPGGameInstanceSubsystem>()->GetSelectedUnitName();

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

	// Create a bunch of unit just to test the map. For now, the initial unit chosed by the player plus 2 allies plus an npc are spawned
	CreateNewUnit(SelectedUnitName, EUnitControllerOwner::PlayerController1, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(200.0f, 0.0f, 20.0f)));
	CreateNewUnit(TEXT("Charmander"), EUnitControllerOwner::PlayerController1, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(1200.0f, 0.0f, 20.0f)));
	CreateNewUnit(TEXT("Squirtle"), EUnitControllerOwner::PlayerController1, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(800.0f, 1600.0f, 20.0f)));
	CreateNewUnit(TEXT("Rattata"), EUnitControllerOwner::NpcController, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(600.0f, 200.0f, 20.0f)));

	StartGame();
}

// Create a new unit, initialize it and save its reference in the UnitsArray
void ATRPGGameStateBase::CreateNewUnit(FName Archetype, EUnitControllerOwner ControllerOwner, FTransform Location)
{
	int32 NewUnitIndex;

	if (ControllerOwner >= EUnitControllerOwner::NpcController)
		NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(NpcUnitClass, Location));
	else
		NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(BaseUnitClass, Location));

	UnitsArray[NewUnitIndex]->Init(Archetype, ControllerOwner, NewUnitIndex);
	UnitsArray[NewUnitIndex]->FinishSpawning(Location);
}

void ATRPGGameStateBase::StartGame()
{
	// Init random seed
	FMath::RandInit(FDateTime::Now().GetTicks());

	// The function TurnStarts is called in all units so their initial stats like armor can be set
	for (ABaseUnit* Unit : UnitsArray)
		Unit->TurnStarts();

	// And the game always starts with the player turn
	ControllerTurn = EUnitControllerOwner::PlayerController1;

	OnGameStarts.Broadcast();
	UE_LOG(LogTemp, Display, TEXT("[GAME STATE] OnNewTurnStarts Broadcast from StartGame()"));
	OnNewTurnStarts.Broadcast(ControllerTurn);
}

void ATRPGGameStateBase::SetNextTurn()
{
	for (ABaseUnit* Unit : UnitsArray)
	{
		check(Unit);
		if (Unit->GetControllerOwner() == ControllerTurn && Unit->IsAlive())
		{
			Unit->TurnEnds();
		}
	}

	// Set the next Player. In this case, the turn rotates between the player and the NpcController.
	if (ControllerTurn == EUnitControllerOwner::PlayerController1)
	{
		UE_LOG(LogTemp, Display, TEXT("[GAME STATE] The AI's turn starts"));
		ControllerTurn = EUnitControllerOwner::NpcController;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[GAME STATE] The Player's turn starts"));
		ControllerTurn = EUnitControllerOwner::PlayerController1;
	}
		
	for (ABaseUnit* Unit : UnitsArray)
	{
		check(Unit);
		if (Unit->GetControllerOwner() == ControllerTurn && Unit->IsAlive())
		{
			Unit->TurnStarts();
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("[GAME STATE] OnNewTurnStarts Broadcast from SetNextTurn()"));
	OnNewTurnStarts.Broadcast(ControllerTurn);
}

void ATRPGGameStateBase::ControllerLostGame(EUnitControllerOwner ControllerOwner)
{
	//TODO For now the system is going to be implemented so always there is a PlayerController and a NpcController. In the future it could be scaled to multiple players
	EUnitControllerOwner WinnerController = EUnitControllerOwner::None;

	if (ControllerOwner == EUnitControllerOwner::NpcController)
	{
		UE_LOG(LogTemp, Display, TEXT("[GAME STATE] Player Won"));
		WinnerController = EUnitControllerOwner::PlayerController1;
	}
	else if(ControllerOwner == EUnitControllerOwner::PlayerController1)
	{
		UE_LOG(LogTemp, Display, TEXT("[GAME STATE] Player Lost"));
		WinnerController = EUnitControllerOwner::NpcController;
	}

	OnGameOver.Broadcast(WinnerController);
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