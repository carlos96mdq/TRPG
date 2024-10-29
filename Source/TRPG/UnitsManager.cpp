// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitsManager.h"
#include "TRPGGameStateBase.h"
#include "TRPGPlayerState.h"
#include "BaseUnit.h"
#include "NpcUnit.h"

DEFINE_LOG_CATEGORY_STATIC(LogUnitsManager, Log, All)

void UUnitsManager::CreateNewUnits(const TArray<FUnitInitData>& UnitsData)
{
    for (const FUnitInitData& UnitData : UnitsData)
    {
        CreateNewUnit(UnitData.Archetype, UnitData.ControllerOwner, UnitData.Transform);
    }
}

void UUnitsManager::CreateNewUnit(FName Archetype, EUnitControllerOwner ControllerOwner, const FTransform& Transform)
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
	int32 NewUnitIndex;

	// Create and save the unit
	if (ControllerOwner >= EUnitControllerOwner::AI1)
		NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(GameState->GetNpcUnitClass(), Transform));
	else
		NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(GameState->GetBaseUnitClass(), Transform));

	UnitsArray[NewUnitIndex]->Init(Archetype, ControllerOwner, NewUnitIndex);
	UnitsArray[NewUnitIndex]->FinishSpawning(Transform);

	// Bind all the units delegate to this actor
	//TODO maybe I should change all the array access [] UnitsArray[NewUnitIndex] for an unique access and a pointer
	UnitsArray[NewUnitIndex]->OnUnitStopsMoving.AddUObject(this, &UUnitsManager::OnUnitStops);
	UnitsArray[NewUnitIndex]->OnUnitStopsAction.AddUObject(this, &UUnitsManager::OnUnitStops);
	UnitsArray[NewUnitIndex]->OnUnitUpdateStats.BindUObject(this, &UUnitsManager::OnUnitUpdateStats);
}

void UUnitsManager::UnitsStartTurn(EUnitControllerOwner ControllerOwner)
{
    for (ABaseUnit* Unit : UnitsArray)
    {
        if ((ControllerOwner == EUnitControllerOwner::None || Unit->GetControllerOwner() == ControllerOwner) && Unit->IsAlive())
            Unit->TurnStarts();
    }
}

void UUnitsManager::UnitsEndTurn(EUnitControllerOwner ControllerOwner)
{
    for (ABaseUnit* Unit : UnitsArray)
    {
        if ((ControllerOwner == EUnitControllerOwner::None || Unit->GetControllerOwner() == ControllerOwner) && Unit->IsAlive())
            Unit->TurnEnds();
    }
}

void UUnitsManager::OnUnitUpdateStats(int32 UnitIndex)
{
    if (UnitsArray.IsValidIndex(UnitIndex))
    {
        ABaseUnit* Unit = UnitsArray[UnitIndex];
        ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

        for (APlayerState* PlayerState : GameState->PlayerArray)
        {
            if (ATRPGPlayerState* MyPlayerState = Cast<ATRPGPlayerState>(PlayerState))
                MyPlayerState->OnUnitUpdateStats(Unit);
        }

        if (!Unit->IsAlive())
        {
            bool bAllPlayersAreDead = true;
            EUnitControllerOwner ControllerOwner = Unit->GetControllerOwner();

            for (const ABaseUnit* UnitToCheck : UnitsArray)
            {
                if (UnitToCheck->GetControllerOwner() == ControllerOwner && UnitToCheck->IsAlive())
                {
                    bAllPlayersAreDead = false;
                    break;
                }
            }

            if (bAllPlayersAreDead)
                GameState->ControllerLostGame(ControllerOwner);
        }
    }
    else
        UE_LOG(LogUnitsManager, Display, TEXT("Tried to access an invalid Unit with index %d"), UnitIndex);
}

void UUnitsManager::OnUnitStops(int32 UnitIndex)
{
    if (UnitsArray.IsValidIndex(UnitIndex))
    {
        ABaseUnit* Unit = UnitsArray[UnitIndex];
        ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

        for (APlayerState* PlayerState : GameState->PlayerArray)
        {
            if (ATRPGPlayerState* MyPlayerState = Cast<ATRPGPlayerState>(PlayerState))
                MyPlayerState->OnUnitUpdateStats(Unit);
        }
    }
}

TArray<FVector> UUnitsManager::GetAllUnitLocations()
{
    TArray<FVector> Locations;

    for (ABaseUnit* Unit : UnitsArray)
    {
        if (Unit->IsAlive())
        {
            Locations.Emplace(Unit->GetActorLocation());
        }
    }
    return Locations;
}

ABaseUnit* UUnitsManager::GetUnitByIndex(int32 Index)
{
    if (Index >= 0 && Index < UnitsArray.Num() && UnitsArray[Index] && UnitsArray[Index]->IsAlive())
    {
        return UnitsArray[Index];
    }
    else
    {
        return nullptr;
    }
}

int32 UUnitsManager::GetUnitsNum() const
{
    return UnitsArray.Num();
}