// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/NpcUnit.h"

void ANpcUnit::TurnStarts()
{
	Super::TurnStarts();

	SetAiState(EAiState::WaitingForOrders);
}

void ANpcUnit::TestFunction()
{
	// If the unit was already attacking other unit, try to attack it again if still in range
	if (CurrentObjective != nullptr)
	{


	}


	//ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
	//check(GameState);
	//TODO acorde al behaviour elijo que hacer


	//TODO la combat action debera depender de mucahs cosas, por defecto seteo la primera que sera un ataque
	
	// Try to find an enemy to attack
	//SetCombatAction(0);
	//CurrentState = EUnitState::ReadyToCombat;
	//GameState->GetTerrain()->SetAvailableTiles(this, false);
	//TArray<FVector> UnitsLocations;
	//GameState->GetAllUnitLocations(UnitsLocations);

	


}

void ANpcUnit::SetAiState(EAiState NewState)
{
	AiState = NewState;
}

void ANpcUnit::SetCurrentObjective(ABaseUnit* NewObjective)
{
	CurrentObjective = NewObjective;
}