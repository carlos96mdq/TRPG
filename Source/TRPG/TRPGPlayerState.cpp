// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGPlayerState.h"
#include "BaseTile.h"
#include "BaseUnit.h"
#include "Terrain.h"
#include "PlayerStash.h"
#include "TRPGGameStateBase.h"

void ATRPGPlayerState::BeginPlay()
{
	Super::BeginPlay();

	PlayerStash = GetWorld()->SpawnActor<APlayerStash>();

	GetWorld()->GetGameState<ATRPGGameStateBase>()->OnActiveUnitSet.AddUObject(this, &ATRPGPlayerState::SetActiveUnit);
}

void ATRPGPlayerState::SetTerrain(ATerrain* TerrainPointer)
{
	check(TerrainPointer)
	Terrain = TerrainPointer;
}

void ATRPGPlayerState::SetActiveUnit(ABaseUnit* NewActiveUnit)
{
	ActiveUnit = NewActiveUnit;
}

void ATRPGPlayerState::ProcessClick(FHitResult& HitResult)
{
	// If there is not yet an active unit, it means the the game has nor started yet
	// TODO: deberia agregar en esta parte el chequeo de que la ActiveUnit sea una Unit del Player, sino tampoco hace nada
	if (ActiveUnit == nullptr)
		return;

	// First depending if a unit is selected what happens
	if (ActiveUnit->GetUnitState() == UnitState::ReadyToMove || ActiveUnit->GetUnitState() == UnitState::ReadyToCombat)
	{
		// Process the click handled by the PlayerController according to the unit state
		switch (ActiveUnit->GetUnitState())
		{
		case UnitState::ReadyToMove:
			if (ABaseTile* DestinationTile = Cast<ABaseTile>(HitResult.GetActor()))
			{
				if (Terrain->CheckAvailableTile(DestinationTile))
				{
					TArray<FVector> TilesPath = Terrain->GetPath(DestinationTile);
					Terrain->CleanAvailableTiles();
					ActiveUnit->MoveUnit(TilesPath);
					ActiveUnit->SetUnitState(UnitState::Moving);
					OnStateChanged.Broadcast(UnitState::Moving);
				}
			}
			break;
		case UnitState::ReadyToCombat:
			if (ABaseUnit* HitUnit = Cast<ABaseUnit>(HitResult.GetActor()))
			{
				if (Terrain->CheckAvailableTile(HitUnit->GetActorLocation()))
				{
					ActiveUnit->SetUnitState(UnitState::Combating);
					OnStateChanged.Broadcast(UnitState::Combating);
					ActiveUnit->UseCurrentAction(HitUnit);
					Terrain->CleanAvailableTiles();
					ActiveUnit->SetUnitState(UnitState::Idle);
				}
			}
			break;
		default:
			break;
		}
	}
	// The active unit isn't doing anything, so an unit could be clicked to become the selected unit and be shown in the SlectedUnitWidget
	else
	{
		// If hit a unit, take it as the selected one and let it know to all binded actors
		if (ABaseUnit* HitUnit = Cast<ABaseUnit>(HitResult.GetActor()))
		{
			// TODO: Chequear que la unidad seleccionada no sea la ActiveUnit
			//if (HitUnit->GetUnitState() == UnitState::Idle)
			//{
				SelectedUnit = HitUnit;
				OnUnitSelected.Broadcast(SelectedUnit);
			//}
		}
		else
		{
			SelectedUnit = nullptr;
			OnUnitSelected.Broadcast(SelectedUnit);
		}
	}
}

void ATRPGPlayerState::ReverseState(UUserWidget* ActiveUnitWidget)
{
	// If there is an active unit doing something the reverse function is for they, else, it is for deselect the selcted unit
	if (ActiveUnit && (ActiveUnit->GetUnitState() == UnitState::ReadyToMove || ActiveUnit->GetUnitState() == UnitState::ReadyToCombat))
	{
		ChangeState(UnitState::Idle);
	}	
	else if (SelectedUnit)
	{
		SelectedUnit = nullptr;
		OnUnitSelected.Broadcast(SelectedUnit);
	}
}

void ATRPGPlayerState::ChangeState(UnitState NewState, int32 ActionPosition)
{
	// If there is not yet an active unit, it means the the game has nor started yet
	if (ActiveUnit == nullptr)
		return;

	// Change the CurrentState and reflects it ingame
	switch (NewState)
	{
	case UnitState::Idle:
		Terrain->CleanAvailableTiles();
		ActiveUnit->SetUnitState(NewState);
		break;
	case UnitState::ReadyToMove:
		ActiveUnit->SetUnitState(NewState);
		Terrain->ShowAvailableTiles(ActiveUnit);
		break;
	case UnitState::ReadyToCombat:
		ActiveUnit->SetCombatAction(ActionPosition);
		ActiveUnit->SetUnitState(NewState);
		Terrain->ShowAvailableTiles(ActiveUnit);
		break;
	default:
		break;
	}
}

void ATRPGPlayerState::CheckMovement(UUserWidget* ActiveUnitWidget)
{
	UE_LOG(LogTemp, Warning, TEXT("CheckMovement function called"));
}

