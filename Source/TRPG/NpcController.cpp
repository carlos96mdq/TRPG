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

    for (int32 i = 0; i < GameState->GetUnitsNum(); i++)
    {
        ANpcUnit* Unit = Cast<ANpcUnit>(GameState->GetUnitByIndex(i));
        
        // Register all the units that are npcs to keep a pointer to them and iterate that array when needed
        if (Unit && Unit->GetControllerOwner() == ControllerOwnerName)
        {
            Unit->OnUnitStopsMoving.AddUObject(this, &ANpcController::OnUnitStops);
            Unit->OnUnitStopsAction.AddUObject(this, &ANpcController::OnUnitStops);
        }
    }
}

void ANpcController::NewTurnStarts(EUnitControllerOwner ControllerTurn)
{
    bIsNpcTurn = ControllerTurn == ControllerOwnerName;
    UE_LOG(LogNpcController, Display, TEXT("NewTurnStarts() with the bIsNpcTurn of %s"), (bIsNpcTurn ? TEXT("true") : TEXT("false")));

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

    for (int32 j = 0; j < GameState->GetUnitsNum(); j++)
    {
        if (ANpcUnit* NpcUnit = Cast<ANpcUnit>(GameState->GetUnitByIndex(j)))
        {
            if (NpcUnit->GetControllerOwner() == ControllerOwnerName && NpcUnit->IsAlive() && NpcUnit->GetAiState() == EAiState::WaitingForOrders)
            {
                // First set the unit in combat mode and try to find an objective to attack
                NpcUnit->SetCombatAction(0);
                NpcUnit->SetUnitState(EUnitState::ReadyToCombat);
                GameState->GetTerrain()->SetAvailableTiles(NpcUnit, false);
                ABaseUnit* ObjectiveUnit = NpcUnit->GetCurrentObjective();

                // Check if the previous objective still in range, otherwise try to find a new one
                if (ObjectiveUnit == nullptr || !(ObjectiveUnit->IsAlive()) || !(GameState->GetTerrain()->CheckAvailableTile(ObjectiveUnit->GetActorLocation())))
                {
                    UE_LOG(LogNpcController, Display, TEXT("The npc %s doesn't have a valid Objective, so try to find one"), *(NpcUnit->GetUnitName()).ToString());
                    ObjectiveUnit = nullptr;
                    int32 PossibleObjectiveUnitDistance = -1;

                    for (int32 i = 0; i < GameState->GetUnitsNum(); i++)
                    {
                        if (ABaseUnit* PossibleObjectiveUnit = GameState->GetUnitByIndex(i))
                        {
                            if (PossibleObjectiveUnit->GetControllerOwner() != ControllerOwnerName && GameState->GetTerrain()->CheckAvailableTile(PossibleObjectiveUnit->GetActorLocation()))
                            {
                                int32 TileCost = GameState->GetTerrain()->GetTileCost(PossibleObjectiveUnit->GetActorLocation());
                                if (PossibleObjectiveUnitDistance < 0 || TileCost < PossibleObjectiveUnitDistance)
                                {
                                    PossibleObjectiveUnitDistance = TileCost;
                                    ObjectiveUnit = PossibleObjectiveUnit;
                                }
                            }
                        }
                    }

                    if (ObjectiveUnit)
                        UE_LOG(LogNpcController, Display, TEXT("The npc %s found a valid Objective with name %s"), *(NpcUnit->GetUnitName()).ToString(), *(ObjectiveUnit->GetUnitName()).ToString());

                    NpcUnit->SetCurrentObjective(ObjectiveUnit);
                }

                // Now, only if a valid objective was found, try to use the attack (inside the attempt function, the energy is checked)
                if (ObjectiveUnit)
                {
                    UE_LOG(LogNpcController, Display, TEXT("The npc %s tries to use an action"), *(NpcUnit->GetUnitName()).ToString());
                    if (NpcUnit->TryUsingCurrentAction(ObjectiveUnit))
                    {
                        UE_LOG(LogNpcController, Display, TEXT("The npc %s used an action"), *(NpcUnit->GetUnitName()).ToString());
                        bAllNpcAreSleeping = false;
                        break;
                    }
                    else
                    {
                        UE_LOG(LogNpcController, Display, TEXT("The npc %s failed using an action"), *(NpcUnit->GetUnitName()).ToString());
                    }
                }
                else
                {
                    // In this case the unit couldn't find any objective in range. So, the npc is going to try to move so it can find a objective further away objective
                    if (ABaseUnit* NearestEnemy = GameState->GetTerrain()->FindNearestEnemy(NpcUnit))
                    {
                        UE_LOG(LogNpcController, Display, TEXT("Found Unit"));
                        NpcUnit->SetCurrentObjective(NearestEnemy);

                        // Try to get as near as possible to this unit according to the left energy in the npc
                        NpcUnit->SetUnitState(EUnitState::ReadyToMove);
                        GameState->GetTerrain()->SetAvailableTiles(NpcUnit, false);

                        // TODO The 2 offset is a "temporal solution". The ideal way would be to check the npc attack and its min range, and determinate how many
                        // tiles away the npc should be to attack. But in this case we assume that it use the same attack with the same min range always and set 
                        // the code to work with it
                        TArray<FVector> TilesPath = GameState->GetTerrain()->GetPath(NearestEnemy->GetActorLocation(), 2);

                        if (!TilesPath.IsEmpty())
                        {
                            NpcUnit->MoveUnit(TilesPath);
                            GameState->GetTerrain()->CleanAvailableTiles();
                            bAllNpcAreSleeping = false;
                            break;
                        }
                    }
                }
                
                // If we get until here without doing anything the unit can't do anything more and just end its actions this turn
                UE_LOG(LogNpcController, Display, TEXT("The npc %s couldn't make anything, it is going to sleep"), *(NpcUnit->GetUnitName()).ToString());
                NpcUnit->SetUnitState(EUnitState::Idle);
                NpcUnit->SetAiState(EAiState::Sleeping);
                GameState->GetTerrain()->CleanAvailableTiles();
            }
        }
    }

    if (bAllNpcAreSleeping)
    {
        UE_LOG(LogNpcController, Display, TEXT("All npcs are sleepings so the Ai ends it turn"));
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

    // The unit doesn't exist anymore, maybe because it died
    if (Unit == nullptr)
    {
        bool bAllUnitsAreDead = true;

        for (int32 i = 0; i < GameState->GetUnitsNum(); i++)
        {
            if (ABaseUnit* UnitToCheck = GameState->GetUnitByIndex(i))
            {
                if (UnitToCheck->GetControllerOwner() == ControllerOwnerName)
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