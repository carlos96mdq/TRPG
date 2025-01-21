// Fill out your copyright notice in the Description page of Project Settings.


#include "System/TRPGGameStateBase.h"
#include "Units/BaseUnit.h"
#include "Units/Terrain.h"
#include "Units/UnitsManager.h"
#include "Units/NpcController.h"
#include "System/TRPGPlayerState.h"
#include "System/TRPGGameInstanceSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(LogTRPGGameState, Log, All)

ATRPGGameStateBase::ATRPGGameStateBase()
{
	// TODO Only for building purpose, this array is going to be hardcoded instead of being filled from a Data Table
	DamageTypeModifiers = {
		//None  Bug  Dark  Drag  Elec  Fairy Fight Fire  Fly   Ghost Grass Grou  Norm  Ice   Pois  Psych Rock  Steel Water 
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f },	// None
		{ 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 2.0f, 1.0f, 1.0f, 1.0f, 0.5f, 2.0f, 1.0f, 0.5f, 1.0f },	// Bug
		{ 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f },	// Dark
		{ 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f },	// Dragon
		{ 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f },	// Electric
		{ 1.0f, 1.0f, 2.0f, 2.0f, 1.0f, 1.0f, 2.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, 1.0f },	// Fairy
		{ 1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.5f, 0.0f, 1.0f, 1.0f, 2.0f, 2.0f, 0.5f, 0.5f, 2.0f, 2.0f, 1.0f },	// Fighting
		{ 1.0f, 2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 0.5f, 2.0f, 0.5f },	// Fire
		{ 1.0f, 2.0f, 1.0f, 1.0f, 0.5f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 1.0f },	// Flying
		{ 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f },	// Ghost
		{ 1.0f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 0.5f, 1.0f, 2.0f, 0.5f, 2.0f },	// Grass
		{ 1.0f, 0.5f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 2.0f, 0.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 2.0f, 2.0f, 1.0f },	// Ground
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, 1.0f },	// Normal
		{ 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.5f, 2.0f, 1.0f, 2.0f, 2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f },	// Ice
		{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.5f, 2.0f, 0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.5f, 0.0f, 1.0f },	// Poison
		{ 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 0.5f, 1.0f, 0.5f, 1.0f },	// Psychic
		{ 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 2.0f, 2.0f, 1.0f, 1.0f, 0.5f, 1.0f, 2.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f },	// Rock
		{ 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 2.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 2.0f, 0.5f, 0.5f },	// Steel
		{ 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 1.0f, 0.5f, 2.0f, 1.0f, 1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 0.5f }	// Water
	};
}

// Called when the game starts or when spawned
void ATRPGGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	
	Terrain = NewObject<UTerrain>(this);
	UnitsManager = NewObject<UUnitsManager>(this);
	NpcController = GetWorld()->SpawnActor<ANpcController>();

	GetWorld()->GetFirstPlayerController()->GetPlayerState<ATRPGPlayerState>()->SetTerrain(Terrain);

	// Get Unit selected by Player in MainMenu
	//FName SelectedUnitName = FName("Bulbasaur");	//TODO eliminar esto y dejar el de abajo
	FName SelectedUnitName = GetGameInstance()->GetSubsystem<UTRPGGameInstanceSubsystem>()->GetSelectedUnitName();

	//TODO For now the creation of the Type array from a Data Table was changed for a hardcoded TArray until a better and optimal way is found
	//// Load the Damage Type Modifiers datatable in a local array
	//if (DamageTypeModifiersTable != nullptr)
	//{
	//	TArray<FDamageTypeModifiers*> AttackerTypesRows;
	//	DamageTypeModifiersTable->GetAllRows<FDamageTypeModifiers>("", AttackerTypesRows);
	//	
	//	
	//	
	//	
	//	TArray<TArray<FString>> WARDTArray = DamageTypeModifiersTable->GetTableData();
	//	TArray<float> InitializationArray;
	//	InitializationArray.Init(1, (int32)EUnitType::MAX);
	//	DamageTypeModifiers.Init(InitializationArray, (int32)EUnitType::MAX);

	//	for (int32 Attacker = 1; Attacker < WARDTArray.Num(); Attacker++)
	//	{
	//		for (int32 Defender = 1; Defender < WARDTArray.Num(); Defender++)
	//		{
	//			DamageTypeModifiers[Attacker][Defender] = FCString::Atof(*(WARDTArray[Attacker][Defender]));
	//		}
	//	}
	//}
	//else
	//{
	//	UE_LOG(LogGameState, Warning, TEXT("The Damage Type Modifiers Table wasn't found"));
	//}

	Terrain->CreateMap();

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
	
	UE_LOG(LogGameState, Display, TEXT("OnNewTurnStarts Broadcast from StartGame()"));
	OnNewTurnStarts.Broadcast(ControllerTurn);
}

void ATRPGGameStateBase::SetNextTurn()
{
	UnitsManager->UnitsEndTurn(ControllerTurn);

	// Set the next Player. In this case, the turn rotates between the player and the NpcController.
	if (ControllerTurn == EUnitControllerOwner::Player1)
	{
		UE_LOG(LogGameState, Display, TEXT("The AI's turn starts"));
		ControllerTurn = EUnitControllerOwner::AI1;
	}
	else
	{
		UE_LOG(LogGameState, Display, TEXT("The Player's turn starts"));
		ControllerTurn = EUnitControllerOwner::Player1;
	}

	UnitsManager->UnitsStartTurn(ControllerTurn);

	UE_LOG(LogGameState, Display, TEXT("OnNewTurnStarts Broadcast from SetNextTurn()"));
	OnNewTurnStarts.Broadcast(ControllerTurn);
}

void ATRPGGameStateBase::ControllerLostGame(EUnitControllerOwner ControllerOwner)
{
	// System implemented for a one player game
	EUnitControllerOwner WinnerController = EUnitControllerOwner::None;

	if (ControllerOwner == EUnitControllerOwner::AI1)
	{
		UE_LOG(LogGameState, Display, TEXT("Player Won"));
		WinnerController = EUnitControllerOwner::Player1;
	}
	else if (ControllerOwner == EUnitControllerOwner::Player1)
	{
		UE_LOG(LogGameState, Display, TEXT("Player Lost"));
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

int32 ATRPGGameStateBase::GetUnitsNum() const
{
	return UnitsManager->GetUnitsNum();
}

float ATRPGGameStateBase::GetDamageTypeModifier(int32 DamageType, int32 DefenderType, int32 DefenderSubType) const
{
	float ResistanceValue = DamageTypeModifiers[DamageType][DefenderType] * DamageTypeModifiers[DamageType][DefenderSubType];
	return ResistanceValue;
}