// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGPlayerController.h"
#include "BaseUnit.h"
#include "BaseTile.h"
#include "TRPGGameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Terrain.h"
#include "Blueprint/UserWidget.h"
#include "TRPGPlayerState.h"
#include "ActiveUnitWidget.h"
#include "HUDWidget.h"

ATRPGPlayerController::ATRPGPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void ATRPGPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("MouseLeftClicked", IE_Pressed, this, &ATRPGPlayerController::OnMouseLeftClicked);
    InputComponent->BindAction("MouseRightClicked", IE_Pressed, this, &ATRPGPlayerController::OnMouseRightClicked);
}

void ATRPGPlayerController::BeginPlay()
{
    Super::BeginPlay();
    
    HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
    check(HUDWidget);
    HUDWidget->AddToViewport();
}

void ATRPGPlayerController::OnMouseLeftClicked()
{
    if (ATRPGPlayerState* MyPlayerState = GetPlayerState<ATRPGPlayerState>())
    {
        //Get actor hit and send it to PlayerState
        FHitResult HitResult;
        GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, HitResult);
        MyPlayerState->ProcessClick(HitResult);
    }
    else
    {
        checkNoEntry();
    }
}

void ATRPGPlayerController::OnMouseRightClicked()
{
    if (ATRPGPlayerState* MyPlayerState = GetPlayerState<ATRPGPlayerState>())
    {
        //Get actor hit and send it to PlayerState
        MyPlayerState->ReverseState(ActiveUnitWidget);
    }
    else
    {
        checkNoEntry();
    }
}

void ATRPGPlayerController::OnCombatAction(int32 ActionPosition)
{
    UE_LOG(LogTemp, Warning, TEXT("Combat action handled"));
    if (ATRPGPlayerState* MyPlayerState = GetPlayerState<ATRPGPlayerState>())
    {
        MyPlayerState->ChangeState(UnitState::ReadyToCombat, ActionPosition);
    }
    else
    {
        checkNoEntry();
    }
}

void ATRPGPlayerController::OnMoveAction()
{
    UE_LOG(LogTemp, Warning, TEXT("Move action handled"));
    if (ATRPGPlayerState* MyPlayerState = GetPlayerState<ATRPGPlayerState>())
    {
        MyPlayerState->ChangeState(UnitState::ReadyToMove);
    }
    else
    {
        checkNoEntry();
    }
}

void ATRPGPlayerController::OnWaitAction()
{
    UE_LOG(LogTemp, Warning, TEXT("Wait action handled"));
    if (ATRPGPlayerState* MyPlayerState = GetPlayerState<ATRPGPlayerState>())
    {
        MyPlayerState->ChangeState(UnitState::Idle);
        GetWorld()->GetGameState<ATRPGGameStateBase>()->SetNextTurn();
    }
    else
    {
        checkNoEntry();
    }
}