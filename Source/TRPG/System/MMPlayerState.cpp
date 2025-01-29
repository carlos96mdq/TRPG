// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MMPlayerState.h"
#include "System/MMPlayerController.h"
#include "Engine/DataTable.h"
#include "Units/UnitModel.h"
#include "Units/BaseUnit.h"
#include "System/TRPGGameInstanceSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "System/MySaveGame.h"

void AMMPlayerState::BeginPlay()
{
	Super::BeginPlay();

	//TODO esto esta hardcodeado de esta manera para probar. En un futuro, lo ideal seria obtener esta informacion de manera mas certera y dinamica
	// Save units Names in PlayerState
	MyUnits.Emplace("Bulbasaur");
	MyUnits.Emplace("Charmander");
	MyUnits.Emplace("Squirtle");

	// Create selected unit model and spawn in the world
	SelectedUnitModel = GetWorld()->SpawnActor<AUnitModel>(FVector(0.0f, 0.0f, 50.0f), FRotator(0.0f, 0.0f, 0.0f));

	LoadPlayerMatchesRecord();
}

UTexture2D* AMMPlayerState::GetUnitIcon(int32 UnitIndex) const
{
	// Check that the index is inside the array limits
	if (UnitIndex >= 0 && UnitIndex < MyUnits.Num())
	{
		FName UnitName = MyUnits[UnitIndex];

		// Get unit data from data table
		FUnits* BaseStats = UnitsTable->FindRow<FUnits>(UnitName, "", true);
		check(BaseStats);

		// Get unit icon
		return BaseStats->Icon.LoadSynchronous();
	}
	return nullptr;
}

void AMMPlayerState::SetSelectedUnit(int32 UnitIndex)
{
	if (UnitIndex >= 0 && UnitIndex < MyUnits.Num())
	{
		FName UnitName = MyUnits[UnitIndex];
		
		// Get unit data from data table
		FUnits* BaseStats = UnitsTable->FindRow<FUnits>(UnitName, "", true);
		check(BaseStats);

		// Get unit 3D Model
		USkeletalMesh* Model3D = BaseStats->Model3D.LoadSynchronous();

		// Set Model in UnitModel
		SelectedUnitModel->SetMesh(Model3D);

		// Save selected unit name in Game Instance subsystem
		GetGameInstance()->GetSubsystem<UTRPGGameInstanceSubsystem>()->SetSelectedUnitName(UnitName);
	}
}

void AMMPlayerState::LoadPlayerMatchesRecord()
{
	UMySaveGame* SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));

	// If there's already saved data, load it
	if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0))
	{
		SaveGameInstance = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));

		TArray<int> LoadedData = {
			SaveGameInstance->MatchesPlayed,
			SaveGameInstance->MatchesWon,
			SaveGameInstance->MatchesLost,
			SaveGameInstance->UnitsDefeated,
			SaveGameInstance->UnitsLost
		};

		if (AMMPlayerController* MyPlayerController = Cast<AMMPlayerController>(GetPlayerController()))
		{
			MyPlayerController->ShowPlayerMatchesRecordData(LoadedData);
		}
	}
}