// Fill out your copyright notice in the Description page of Project Settings.


#include "System/TRPGPlayerState.h"
#include "Units/BaseTile.h"
#include "Units/BaseUnit.h"
#include "Units/Terrain.h"
#include "System/MySaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "System/TRPGGameStateBase.h"
#include "System/TRPGPlayerController.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerState, Log, All)

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
		{
			AddUnitDefeatedToResults(Unit->GetControllerOwner() == MyPlayerController->GetControllerName());
		}
		
		MyPlayerController->OnUnitUpdateStats(Unit);
	}
}

void ATRPGPlayerState::AddUnitDefeatedToResults(bool PlayerIsOwner)
{
	if (PlayerIsOwner)
	{
		DataToSave.UnitsLost++;
	}
	else
	{
		DataToSave.UnitsDefeated++;
	}
}

void ATRPGPlayerState::AddMatchFinishedToResults(bool PlayerWon)
{
	DataToSave.MatchesPlayed++;
	if (PlayerWon)
	{
		DataToSave.MatchesWon++;
	}
	else
	{
		DataToSave.MatchesLost++;
	}
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

	// Save the data back to the slot
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("PlayerSaveSlot"), 0);
}

void ATRPGPlayerState::SetActiveUnit(ABaseUnit* NewActiveUnit)
{
	ActiveUnit = NewActiveUnit;
}

void ATRPGPlayerState::ProcessClick(FHitResult& HitResult)
{
	// If there is not yet an active unit, it means the the game has nor started yet
	if (ActiveUnit == nullptr)
	{
		return;
	}

	ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
	UTerrain* Terrain = GameState->GetTerrain();

	if (Terrain == nullptr)
	{
		UE_LOG(LogPlayerState, Warning, TEXT("OnNewTurnStarts Broadcast from StartGame()"));
		return;
	}

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
			SelectedUnit = HitUnit;
			OnUnitSelected.Broadcast(SelectedUnit);
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
	{
		return;
	}

	ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
	UTerrain* Terrain = GameState->GetTerrain();
	if (Terrain == nullptr)
	{
		UE_LOG(LogPlayerState, Warning, TEXT("OnNewTurnStarts Broadcast from StartGame()"));
		return;
	}

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
