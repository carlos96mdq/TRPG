// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitsManager.h"
#include "TRPGGameStateBase.h"
#include "TRPGPlayerState.h"
#include "BaseUnit.h"
#include "NpcUnit.h"

DEFINE_LOG_CATEGORY_STATIC(LogUnitsManager, Log, All)

// Sets default values
AUnitsManager::AUnitsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	//TODO if is not necesary, erase

}

// Called when the game starts or when spawned
void AUnitsManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnitsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUnitsManager::CreateNewUnits(const TArray<FUnitInitData>& UnitsData)
{
    for (const FUnitInitData& UnitData : UnitsData)
    {
        CreateNewUnit(UnitData.Archetype, UnitData.ControllerOwner, UnitData.Location);
    }
}

void AUnitsManager::CreateNewUnit(FName Archetype, EUnitControllerOwner ControllerOwner, FTransform Location)
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
	int32 NewUnitIndex;

	// Create and save the unit
	if (ControllerOwner >= EUnitControllerOwner::AI1)
		NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(GameState->GetNpcUnitClass(), Location));
	else
		NewUnitIndex = UnitsArray.Emplace(GetWorld()->SpawnActorDeferred<ABaseUnit>(GameState->GetBaseUnitClass(), Location));

	UnitsArray[NewUnitIndex]->Init(Archetype, ControllerOwner, NewUnitIndex);
	UnitsArray[NewUnitIndex]->FinishSpawning(Location);

	// Bind all the units delegate to this actor
	//TODO because this is the online actor that shouyld be bind to those delegates, convert them from multicast to onecast
	//TODO maybe I should change all the array access [] UnitsArray[NewUnitIndex] for an unique acces and a pointer
	UnitsArray[NewUnitIndex]->OnUnitStopsMoving.AddUObject(this, &AUnitsManager::OnUnitStops);
	UnitsArray[NewUnitIndex]->OnUnitStopsAction.AddUObject(this, &AUnitsManager::OnUnitStops);
	UnitsArray[NewUnitIndex]->OnUnitUpdateStats.BindUObject(this, &AUnitsManager::OnUnitUpdateStats);
}

void AUnitsManager::UnitsStartTurn(EUnitControllerOwner ControllerOwner)
{
    for (ABaseUnit* Unit : UnitsArray)
    {
        if ((ControllerOwner == EUnitControllerOwner::None || Unit->GetControllerOwner() == ControllerOwner) && Unit->IsAlive())
            Unit->TurnStarts();
    }
}

void AUnitsManager::UnitsEndTurn(EUnitControllerOwner ControllerOwner)
{
    for (ABaseUnit* Unit : UnitsArray)
    {
        if ((ControllerOwner == EUnitControllerOwner::None || Unit->GetControllerOwner() == ControllerOwner) && Unit->IsAlive())
            Unit->TurnEnds();
    }
}

void AUnitsManager::OnUnitUpdateStats(int32 UnitIndex)
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

void AUnitsManager::OnUnitStops(int32 UnitIndex)
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
        //TODO should see how to invoqe the correct function in the PlayerController or NpcController when needed
        //if (PlayerUnitIndex == PlayerActiveUnitIndex)
        //{
        //    OnMoveAction();
        //    HUDWidget->UpdateActiveUnitData(Unit);
        //}
    }
}

TArray<FVector> AUnitsManager::GetAllUnitLocations()
{
    TArray<FVector> Locations;

    for (ABaseUnit* Unit : UnitsArray)
        Locations.Emplace(Unit->GetActorLocation());

    return Locations;
}

ABaseUnit* AUnitsManager::GetUnitByIndex(int32 Index)
{
    if (Index >= 0 && Index < UnitsArray.Num())
        return UnitsArray[Index];
    else
        return nullptr;
}

int32 AUnitsManager::GetUnitsQuantity() const
{
    return UnitsArray.Num();
}