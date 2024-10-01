// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGPlayerState.h"
#include "BaseTile.h"
#include "BaseUnit.h"
#include "Terrain.h"
#include "MySaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "TRPGGameStateBase.h"
#include "TRPGPlayerController.h"

void ATRPGPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	GetWorld()->GetGameState<ATRPGGameStateBase>()->OnActiveUnitSet.AddUObject(this, &ATRPGPlayerState::SetActiveUnit);
}

void ATRPGPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ATRPGPlayerState::OnUnitUpdateStats(ABaseUnit* Unit)
{
	if (ATRPGPlayerController* MyPlayerController = Cast<ATRPGPlayerController>(GetPlayerController()))
	{
		if (!Unit->IsAlive())
			AddUnitDefeatedToResults(Unit->GetControllerOwner() == MyPlayerController->GetControllerName());
		
		MyPlayerController->OnUnitUpdateStats(Unit);
	}
}

void ATRPGPlayerState::AddUnitDefeatedToResults(bool PlayerIsOwner)
{
	if (PlayerIsOwner)
		DataToSave.UnitsLost++;
	else
		DataToSave.UnitsDefeated++;
}

void ATRPGPlayerState::AddMatchFinishedToResults(bool PlayerWon)
{
	DataToSave.MatchesPlayed++;
	if (PlayerWon)
		DataToSave.MatchesWon++;
	else
		DataToSave.MatchesLost++;
}

void ATRPGPlayerState::SavePlayerResults()
{
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	// If there's already saved data, load it
	if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
	{
		SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));
	}

	// Update save data
	SaveGameInstance->MatchesPlayed += DataToSave.MatchesPlayed;
	SaveGameInstance->MatchesWon += DataToSave.MatchesWon;
	SaveGameInstance->MatchesLost += DataToSave.MatchesLost;
	SaveGameInstance->UnitsDefeated += DataToSave.UnitsDefeated;
	SaveGameInstance->UnitsLost += DataToSave.UnitsLost;

	UE_LOG(LogTemp, Display, TEXT("[PLAYER STATE] The values of MatchesPlayed %d, MatchesWon = %d and MatchesLost = %d were saved"), SaveGameInstance->MatchesPlayed, SaveGameInstance->MatchesWon, SaveGameInstance->MatchesLost);

	// Save the data back to the slot
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0);
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
	if (ActiveUnit->GetUnitState() == EUnitState::ReadyToMove || ActiveUnit->GetUnitState() == EUnitState::ReadyToCombat)
	{
		// Process the click handled by the PlayerController according to the unit state
		switch (ActiveUnit->GetUnitState())
		{
		case EUnitState::ReadyToMove:
			if (ABaseTile* DestinationTile = Cast<ABaseTile>(HitResult.GetActor()))
			{
				if (Terrain->CheckAvailableTile(DestinationTile))
				{
					TArray<FVector> TilesPath = Terrain->GetPath(DestinationTile);
					Terrain->CleanAvailableTiles();
					ActiveUnit->MoveUnit(TilesPath);
					ActiveUnit->SetUnitState(EUnitState::Moving);
					OnStateChanged.Broadcast(EUnitState::Moving);
				}
			}
			break;
		case EUnitState::ReadyToCombat:
			if (ABaseUnit* HitUnit = Cast<ABaseUnit>(HitResult.GetActor()))
			{
				if (Terrain->CheckAvailableTile(HitUnit->GetActorLocation()))
				{
					ActiveUnit->SetUnitState(EUnitState::Combating);
					OnStateChanged.Broadcast(EUnitState::Combating);
					ActiveUnit->UseCurrentAction(HitUnit);
					Terrain->CleanAvailableTiles();
					ActiveUnit->SetUnitState(EUnitState::Idle);
				}
			}
			else if (ABaseTile* HitTile = Cast<ABaseTile>(HitResult.GetActor()))
			{
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
	if (ActiveUnit && (ActiveUnit->GetUnitState() == EUnitState::ReadyToMove || ActiveUnit->GetUnitState() == EUnitState::ReadyToCombat))
	{
		ChangeState(EUnitState::Idle);
	}	
	else if (SelectedUnit)
	{
		SelectedUnit = nullptr;
		OnUnitSelected.Broadcast(SelectedUnit);
	}
}

void ATRPGPlayerState::ChangeState(EUnitState NewState, int32 ActionPosition)
{
	// If there is not yet an active unit, it means the the game has nor started yet
	if (ActiveUnit == nullptr)
		return;

	// Change the CurrentState and reflects it ingame
	switch (NewState)
	{
	case EUnitState::Idle:
		Terrain->CleanAvailableTiles();
		ActiveUnit->SetUnitState(NewState);
		break;
	case EUnitState::ReadyToMove:
		ActiveUnit->SetUnitState(NewState);
		Terrain->SetAvailableTiles(ActiveUnit);
		break;
	case EUnitState::ReadyToCombat:
		ActiveUnit->SetCombatAction(ActionPosition);
		ActiveUnit->SetUnitState(NewState);
		Terrain->SetAvailableTiles(ActiveUnit);
		break;
	default:
		break;
	}
}

void ATRPGPlayerState::CheckMovement(UUserWidget* ActiveUnitWidget)
{
}

