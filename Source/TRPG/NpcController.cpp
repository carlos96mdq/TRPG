// Fill out your copyright notice in the Description page of Project Settings.

#include "NpcController.h"
#include "TRPGGameStateBase.h"
#include "Terrain.h"
#include "NpcUnit.h"

DEFINE_LOG_CATEGORY_STATIC(LogNpcController, Log, All)

ANpcController::ANpcController()
{
    // By default set this as the NpcController. In the future, multiples Npc could exist so each NpcController should be set with its own enum id
    ControllerOwnerName = EUnitControllerOwner::AI1;
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANpcController::BeginPlay()
{
	Super::BeginPlay();
	
    GetWorld()->GetGameState<ATRPGGameStateBase>()->OnGameStarts.AddUObject(this, &ANpcController::RegisterAllNpcs);
	GetWorld()->GetGameState<ATRPGGameStateBase>()->OnNewTurnStarts.AddUObject(this, &ANpcController::NewTurnStarts);
    GetWorld()->GetGameState<ATRPGGameStateBase>()->OnGameOver.AddUObject(this, &ANpcController::GameOver);

    SetActorTickEnabled(false);
}

// Called every frame
void ANpcController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bCheckForNextAction)
    {
        bCheckForNextAction = false;
        SetActorTickEnabled(false);
        CheckForNextAction();
    }
}

void ANpcController::RegisterAllNpcs()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

    for (int32 i = 0; i < GameState->GetUnitsQuantity(); i++)
    {
        ANpcUnit* Unit = Cast<ANpcUnit>(GameState->GetUnitByIndex(i));
        
        // Register all the units that are npcs to keep a pointer to them and iterate that array when needed
        if (Unit != nullptr && Unit->GetControllerOwner() == ControllerOwnerName)
            Unit->OnUnitStopsAction.AddUObject(this, &ANpcController::OnUnitStops);
    }
}

void ANpcController::NewTurnStarts(EUnitControllerOwner ControllerTurn)
{
    bIsNpcTurn = ControllerTurn == ControllerOwnerName;
    UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] NewTurnStarts() with the bIsNpcTurn of %s"), (bIsNpcTurn ? TEXT("true") : TEXT("false")));

    if (bIsNpcTurn)
    {
        bCheckForNextAction = true;
        SetActorTickEnabled(true);
    }
}

void ANpcController::CheckForNextAction()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

    bool bAllNpcAreSleeping = true;

    for (int32 j = 0; j < GameState->GetUnitsQuantity(); j++)
    {
        if (ANpcUnit* NpcUnit = Cast<ANpcUnit>(GameState->GetUnitByIndex(j)))
        {
            if (NpcUnit->GetControllerOwner() == ControllerOwnerName && NpcUnit->IsAlive() && NpcUnit->GetAiState() == EAiState::WaitingForOrders)
            {
                UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] The npc %s is going to be check by the Npc Controller"), *(NpcUnit->GetUnitName()).ToString());
                //TODO acorde al behaviour elijo que hacer
                //TODO la combat action debera depender de mucahs cosas, por defecto seteo la primera que sera un ataque

                // Set unit into combat mode and try to find a unit to attack
                NpcUnit->SetCombatAction(0);
                NpcUnit->SetUnitState(EUnitState::ReadyToCombat);
                GameState->GetTerrain()->SetAvailableTiles(NpcUnit, false);
                ABaseUnit* ObjectiveUnit = NpcUnit->GetCurrentObjective();

                // Check if the previous objective still in range, otherwise try to find a new one
                if (ObjectiveUnit == nullptr || !(ObjectiveUnit->IsAlive()) || !(GameState->GetTerrain()->CheckAvailableTile(ObjectiveUnit->GetActorLocation())))
                {
                    UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] The npc %s doesn't have a valid Objective, so try to find one"), *(NpcUnit->GetUnitName()).ToString());
                    ObjectiveUnit = nullptr;
                    ABaseUnit* PossibleObjectiveUnit = nullptr;
                    int32 PossibleObjectiveUnitDistance = -1;

                    for (int32 i = 0; i < GameState->GetUnitsQuantity(); i++)
                    {
                        PossibleObjectiveUnit = GameState->GetUnitByIndex(i);

                        if (PossibleObjectiveUnit->GetControllerOwner() != ControllerOwnerName && PossibleObjectiveUnit->IsAlive() && GameState->GetTerrain()->CheckAvailableTile(PossibleObjectiveUnit->GetActorLocation()))
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
                        UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] The npc %s found a valid Objective with name %s"), *(NpcUnit->GetUnitName()).ToString(), *(ObjectiveUnit->GetUnitName()).ToString());

                    NpcUnit->SetCurrentObjective(ObjectiveUnit);
                }

                // Now, only if a valid objective was found, try to use the attack. The trying of using it could fail if the Unit hasn't energy enough
                if (ObjectiveUnit)
                {
                    UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] The npc %s tries to use an action"), *(NpcUnit->GetUnitName()).ToString());
                    if (NpcUnit->TryUsingCurrentAction(ObjectiveUnit))
                    {
                        UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] The npc %s used an action"), *(NpcUnit->GetUnitName()).ToString());
                        bAllNpcAreSleeping = false;
                        break;
                    }
                    else
                    {
                        UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] The npc %s failed using an action"), *(NpcUnit->GetUnitName()).ToString());
                    }
                }

                //TODO deberia ver el tema del movimiento
                // There aren't any enemy in range to attack, so set the unit in movement mode and try to find a new unit to move nearby to attack later
                //NpcUnit->SetUnitState(EUnitState::ReadyToMove);
                //GameState->GetTerrain()->SetAvailableTiles(NpcUnit, false);
                //NearestUnitDistance = -1;

                // If we get until here without doing anything the unit can't do anything more and just end its actions this turn
                UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] The npc %s couldn't make anything, it is going to sleep"), *(NpcUnit->GetUnitName()).ToString());
                NpcUnit->SetUnitState(EUnitState::Idle);
                NpcUnit->SetAiState(EAiState::Sleeping);
                GameState->GetTerrain()->CleanAvailableTiles();
            }
        }
    }

    if (bAllNpcAreSleeping)
    {
        UE_LOG(LogTemp, Display, TEXT("[NPC CONTROLLER] All npcs are sleepings so the Ai ends it turn"));
        GameState->SetNextTurn();
    }
}

void ANpcController::GameOver(EUnitControllerOwner WinnerController)
{
    bIsNpcTurn = false;
    bCheckForNextAction = false;
}

void ANpcController::OnUnitStops(int32 UnitIndex)
{
    if (bIsNpcTurn)
    {
        bCheckForNextAction = true;
        SetActorTickEnabled(true);
    }
}

void ANpcController::OnUnitUpdateStats(int32 UnitIndex)
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    ABaseUnit* Unit = GameState->GetUnitByIndex(UnitIndex);
    check(Unit);

    if (!(Unit->IsAlive()))
    {
        bool bAllUnitsAreDead = true;

        for (int32 i = 0; i < GameState->GetUnitsQuantity(); i++)
        {
            if (ABaseUnit* UnitToCheck = GameState->GetUnitByIndex(i))
            {
                if (UnitToCheck->GetControllerOwner() == ControllerOwnerName && UnitToCheck->IsAlive())
                {
                    bAllUnitsAreDead = false;
                    break;
                }
            }
        }

        if (bAllUnitsAreDead)
            GameState->ControllerLostGame(ControllerOwnerName);
    }
}