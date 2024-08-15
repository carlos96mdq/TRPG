// Fill out your copyright notice in the Description page of Project Settings.

#include "NpcController.h"
#include "TRPGGameStateBase.h"
#include "Terrain.h"
#include "NpcUnit.h"

// Sets default values
ANpcController::ANpcController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANpcController::BeginPlay()
{
	Super::BeginPlay();
	
    GetWorld()->GetGameState<ATRPGGameStateBase>()->OnGameStarts.AddUObject(this, &ANpcController::RegisterAllNpcs);
	GetWorld()->GetGameState<ATRPGGameStateBase>()->OnNewTurnStarts.AddUObject(this, &ANpcController::NewTurnStarts);
}

// Called every frame
void ANpcController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANpcController::RegisterAllNpcs()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);

    for (int32 i = 0; i < GameState->GetUnitsQuantity(); i++)
    {
        ANpcUnit* Unit = Cast<ANpcUnit>(GameState->GetUnitByIndex(i));
        
        // Register all the units that are npcs to keep a pointer to them and iterate that array when needed
        if (Unit != nullptr && !Unit->IsPlayer())
        {
            NpcUnits.Emplace(Unit);
            Unit->OnUnitStopsAction.AddUObject(this, &ANpcController::OnUnitStops);
        }
    }
}

void ANpcController::NewTurnStarts(bool bIsPlayer)
{
    bIsNpcTurn = !bIsPlayer;

    if (bIsNpcTurn)
        CheckForNextAction();
}

void ANpcController::CheckForNextAction()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);

    bool bAllNpcSleeping = true;

    for (ANpcUnit* NpcUnit : NpcUnits)
    {
        check(NpcUnit);

        if (NpcUnit->GetAiState() == EAiState::WaitingForOrders)
        {
            UE_LOG(LogTemp, Display, TEXT("[AICONTROLLER] The npc %s is going to be check by the Npc Controller"), *(NpcUnit->GetName()).ToString());
            //TODO acorde al behaviour elijo que hacer
            //TODO la combat action debera depender de mucahs cosas, por defecto seteo la primera que sera un ataque
            
            // Set unit into combat mode and try to find a unit to attack
            NpcUnit->SetCombatAction(0);
            NpcUnit->SetUnitState(EUnitState::ReadyToCombat);
            GameState->GetTerrain()->SetAvailableTiles(NpcUnit, false);
            ABaseUnit* ObjectiveUnit = NpcUnit->GetCurrentObjective();

            // Check if the previous objective still in range, otherwise try to find a new one
            if (ObjectiveUnit == nullptr || !(GameState->GetTerrain()->CheckAvailableTile(ObjectiveUnit->GetActorLocation())))
            {
                UE_LOG(LogTemp, Display, TEXT("[AICONTROLLER] The npc %s doesn't have a valid Objective, so try to find one"), *(NpcUnit->GetName()).ToString());
                ABaseUnit* PossibleObjectiveUnit = nullptr;
                int32 PossibleObjectiveUnitDistance = -1;

                for (int32 i = 0; i < GameState->GetUnitsQuantity(); i++)
                {
                    PossibleObjectiveUnit = GameState->GetUnitByIndex(i);

                    if (PossibleObjectiveUnit->IsPlayer() && PossibleObjectiveUnit->IsAlive() && GameState->GetTerrain()->CheckAvailableTile(PossibleObjectiveUnit->GetActorLocation()))
                    {
                        int32 TileCost = GameState->GetTerrain()->GetTileCost(PossibleObjectiveUnit->GetActorLocation());
                        if (PossibleObjectiveUnitDistance < 0 || TileCost < PossibleObjectiveUnitDistance)
                        {
                            PossibleObjectiveUnitDistance = TileCost;
                            ObjectiveUnit = PossibleObjectiveUnit;
                        }
                    }
                }
                if (ObjectiveUnit)
                {
                    UE_LOG(LogTemp, Display, TEXT("[AICONTROLLER] The npc %s found a valid Objective"), *(NpcUnit->GetName()).ToString());
                    NpcUnit->SetCurrentObjective(ObjectiveUnit);
                }
            }
            
            // Now, only if a valid objective was found, try to use the attack. The trying of using it could fail if the Unit hasn't energy enough
            if (ObjectiveUnit)
            {
                UE_LOG(LogTemp, Display, TEXT("[AICONTROLLER] The npc %s tries to use an action"), *(NpcUnit->GetName()).ToString());
                if (NpcUnit->TryUsingCurrentAction(ObjectiveUnit))
                {
                    UE_LOG(LogTemp, Display, TEXT("[AICONTROLLER] The npc %s used an action"), *(NpcUnit->GetName()).ToString());
                    bAllNpcSleeping = false;
                    break;
                }
                else
                {
                    UE_LOG(LogTemp, Display, TEXT("[AICONTROLLER] The npc %s failed using an action"), *(NpcUnit->GetName()).ToString());
                }
            }
            
            //TODO deberia ver el tema del movimiento
            // There aren't any enemy in range to attack, so set the unit in movement mode and try to find a new unit to move nearby to attack later
            //NpcUnit->SetUnitState(EUnitState::ReadyToMove);
            //GameState->GetTerrain()->SetAvailableTiles(NpcUnit, false);
            //NearestUnitDistance = -1;

            // If we get until here without doing anything the unit can't do anything more and just end its actions this turn
            UE_LOG(LogTemp, Display, TEXT("[AICONTROLLER] The npc %s couldn't make anything, is going to sleep"), *(NpcUnit->GetName()).ToString());
            NpcUnit->SetUnitState(EUnitState::Idle);
            NpcUnit->SetAiState(EAiState::Sleeping);
            GameState->GetTerrain()->CleanAvailableTiles();
        }
    }

    if (bAllNpcSleeping)
    {
        UE_LOG(LogTemp, Display, TEXT("[AICONTROLLER] All npcs are sleepings so the Ai ends it turn"));
        GameState->SetNextTurn();
    }
        
}

void ANpcController::OnUnitStops(int32 UnitIndex)
{
    //TODO en realidad lo que se recibe es el player index. Tengo que ajustar esto para usar un index mas general por sie sto tiene que actualziar el HUD
    CheckForNextAction();
}