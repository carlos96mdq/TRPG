// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGGameStateBase.h"
#include "BaseUnit.h"
#include "Terrain.h"
#include "UnitsManager.h"
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
	UnitsManager = GetWorld()->SpawnActor<AUnitsManager>();
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
	const TArray<FUnitInitData> UnitsInitData = {
		{SelectedUnitName, EUnitControllerOwner::Player1, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(200.0f, 0.0f, 20.0f))},
		{TEXT("Charmander"), EUnitControllerOwner::Player1, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(1200.0f, 0.0f, 20.0f))},
		{TEXT("Squirtle"), EUnitControllerOwner::Player1, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(800.0f, 1600.0f, 20.0f))},
		{TEXT("Rattata"), EUnitControllerOwner::AI1, FTransform(FRotator(0.0f, 0.0f, 0.0f), FVector(600.0f, 200.0f, 20.0f))},
	};
	UnitsManager->CreateNewUnits(UnitsInitData);
	
	StartGame();
}

void ATRPGGameStateBase::StartGame()
{
	// It is important to make this broadcast here so all PlayerController can be set its UnitData before being called by the UnitsManager
	OnGameStarts.Broadcast();

	// Init random seed
	FMath::RandInit(FDateTime::Now().GetTicks());

	// The function TurnStarts is called in all units so their initial stats like armor can be set
	UnitsManager->UnitsStartTurn(EUnitControllerOwner::None);

	// And the game always starts with the player turn
	ControllerTurn = EUnitControllerOwner::Player1;
	
	UE_LOG(LogTemp, Display, TEXT("[GAME STATE] OnNewTurnStarts Broadcast from StartGame()"));
	OnNewTurnStarts.Broadcast(ControllerTurn);
}

void ATRPGGameStateBase::SetNextTurn()
{
	UnitsManager->UnitsEndTurn(ControllerTurn);

	// Set the next Player. In this case, the turn rotates between the player and the NpcController.
	if (ControllerTurn == EUnitControllerOwner::Player1)
	{
		UE_LOG(LogTemp, Display, TEXT("[GAME STATE] The AI's turn starts"));
		ControllerTurn = EUnitControllerOwner::AI1;
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[GAME STATE] The Player's turn starts"));
		ControllerTurn = EUnitControllerOwner::Player1;
	}
		
	UnitsManager->UnitsStartTurn(ControllerTurn);
	
	UE_LOG(LogTemp, Display, TEXT("[GAME STATE] OnNewTurnStarts Broadcast from SetNextTurn()"));
	OnNewTurnStarts.Broadcast(ControllerTurn);
}

void ATRPGGameStateBase::ControllerLostGame(EUnitControllerOwner ControllerOwner)
{
	// System implemented for a one player game
	EUnitControllerOwner WinnerController = EUnitControllerOwner::None;

	if (ControllerOwner == EUnitControllerOwner::AI1)
	{
		UE_LOG(LogTemp, Display, TEXT("[GAME STATE] Player Won"));
		WinnerController = EUnitControllerOwner::Player1;
	}
	else if(ControllerOwner == EUnitControllerOwner::Player1)
	{
		UE_LOG(LogTemp, Display, TEXT("[GAME STATE] Player Lost"));
		WinnerController = EUnitControllerOwner::AI1;
	}

	OnGameOver.Broadcast(WinnerController);
}

TArray<FVector> ATRPGGameStateBase::GetAllUnitLocations() const
{ 
	return UnitsManager->GetAllUnitLocations();
}

ABaseUnit* ATRPGGameStateBase::GetUnitByIndex(int32 Index) const
{ 
	return UnitsManager->GetUnitByIndex(Index);
}

int32 ATRPGGameStateBase::GetUnitsQuantity() const
{
	return UnitsManager->GetUnitsQuantity();
}

float ATRPGGameStateBase::GetDamageTypeModifier(int32 DamageType, int32 DefenderType, int32 DefenderSubType) const
{
	float ResistanceValue = DamageTypeModifiers[DamageType][DefenderType] * DamageTypeModifiers[DamageType][DefenderSubType];
	return ResistanceValue;
}