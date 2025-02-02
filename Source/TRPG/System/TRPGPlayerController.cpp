// Fill out your copyright notice in the Description page of Project Settings.


#include "System/TRPGPlayerController.h"
#include "Units/BaseUnit.h"
#include "Units/BaseTile.h"
#include "System/TRPGGameStateBase.h"
#include "System/TRPGGameModeBase.h"
#include "System/TRPGPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Units/Terrain.h"
#include "UI/HUDWidget.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraActor.h" //TODO al mover a la mainCamera, mover este include con ella

DEFINE_LOG_CATEGORY_STATIC(LogTRPGPlayerController, Log, All)

ATRPGPlayerController::ATRPGPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    // By default set this as the PlayerController. In the future, multiples Players could exist so each PlayerController should be set with its own enum id
    ControllerOwnerName = EUnitControllerOwner::Player1;

    // Spawn this here, so the camera appears in the editor level so it can be positioned there
    /*MainCamera = GetWorld()->SpawnActor<ACameraActor>();*/
}

void ATRPGPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (ATRPGGameStateBase* MyGameState = GetWorld()->GetGameState<ATRPGGameStateBase>())
    {
        MyGameState->OnGameStarts.AddUObject(this, &ATRPGPlayerController::RegisterAllUnits);
        MyGameState->OnNewTurnStarts.AddUObject(this, &ATRPGPlayerController::NewTurnStarts);
        MyGameState->OnGameOver.AddUObject(this, &ATRPGPlayerController::GameOver);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ATRPGGameStateBase couldn't be found"));
    }
}

void ATRPGPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("MouseLeftClicked", IE_Pressed, this, &ATRPGPlayerController::OnMouseLeftClicked);
    InputComponent->BindAction("MouseRightClicked", IE_Pressed, this, &ATRPGPlayerController::OnMouseRightClicked);

    // Add the Input Mapping Context
    if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (!InputMapping.IsNull())
            {
                InputSystem->AddMappingContext(InputMapping.LoadSynchronous(), 0);

                // Bind Move Camera Action
                UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent);
                check(Input);
                UInputAction *MCInputAction = MoveCameraInputAction.LoadSynchronous();
                check(MCInputAction);
                Input->BindAction(MCInputAction, ETriggerEvent::Triggered, this, &ATRPGPlayerController::OnMoveCameraAction);

                // Bind Zoom Camera Action
                UInputAction* ZCInputAction = ZoomCameraInputAction.LoadSynchronous();
                check(ZCInputAction);
                //Input->BindAction(ZCInputAction, ETriggerEvent::Triggered, this, &ATRPGPlayerController::OnZoomCameraAction);
                Input->BindAction(ZCInputAction, ETriggerEvent::Triggered, this, &ATRPGPlayerController::OnZoomCameraAction);
            }
        }
    }

    //TODO esto es horrible aca conceptualmente. el problema es que para crear un Widget en el PlayerController, este debe tener ya un Player seteado
    // y este pasa antes de llamar a SetupInputComponent pero despues de llamar a PostInitializeComponents
    HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
    check(HUDWidget);
    HUDWidget->AddToViewport();
}

void ATRPGPlayerController::BeginPlay()
{
    Super::BeginPlay();

    MainCamera = GetWorld()->SpawnActor<ACameraActor>(FVector(300.0f, 1300.0f, 600.0f), FRotator(-60.0f, -45.0f, 0.0f));
}

void ATRPGPlayerController::SetActiveUnit(ABaseUnit* NewActiveUnit)
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

    if (ABaseUnit* ActiveUnit = GameState->GetUnitByIndex(PlayerActiveUnitIndex))
    {
        ActiveUnit->SetUnitState(EUnitState::Idle);
    }

    PlayerActiveUnitIndex = NewActiveUnit->GetUnitIndex();
    OnMoveAction();

    UE_LOG(LogTRPGPlayerController, Display, TEXT("The unit %s was set as the Player's active unit"), *(NewActiveUnit->GetUnitName()).ToString());

    HUDWidget->UpdateActiveUnitData(NewActiveUnit);
}

void ATRPGPlayerController::RegisterAllUnits()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

    for (int32 i = 0; i < GameState->GetUnitsNum(); i++)
    {
        if (ABaseUnit* Unit = GameState->GetUnitByIndex(i))
        {
            bool IsPlayerUnit = Unit->GetControllerOwner() == ControllerOwnerName;
            HUDWidget->CreateUnitDataWidget(Unit, UnitDataIconClass, IsPlayerUnit);

            if (IsPlayerUnit)
            {
                Unit->OnUnitStopsMoving.AddUObject(this, &ATRPGPlayerController::OnUnitStops);
                Unit->OnUnitStopsAction.AddUObject(this, &ATRPGPlayerController::OnUnitStops);
            }
        }
    }
}

void ATRPGPlayerController::NewTurnStarts(EUnitControllerOwner ControllerTurn)
{
    bIsPlayerTurn = ControllerTurn == ControllerOwnerName;
    UE_LOG(LogTRPGPlayerController, Display, TEXT("NewTurnStarts() with the bIsPlayerTurn of %s"), (bIsPlayerTurn ? TEXT("true") : TEXT("false")));

    HUDWidget->SetPlayerTurn(bIsPlayerTurn);

    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

    if (ControllerTurn == ControllerOwnerName)
    {
        for (int32 i = 0; i < GameState->GetUnitsNum(); i++)
        {
            if (ABaseUnit* Unit = GameState->GetUnitByIndex(i))
            {
                if (Unit->GetControllerOwner() == ControllerOwnerName)
                {
                    SetActiveUnit(Unit);
                    break;
                }
            }
        }
    }
    else
    {
        GameState->GetTerrain()->CleanAvailableTiles();
    }
}

void ATRPGPlayerController::GameOver(EUnitControllerOwner WinnerController)
{
    bIsPlayerTurn = false;
    bool bPlayerWon = WinnerController == ControllerOwnerName;

    ATRPGPlayerState* MyPlayerState = GetPlayerState<ATRPGPlayerState>();
    check(MyPlayerState);
    MyPlayerState->AddMatchFinishedToResults(bPlayerWon);
    MyPlayerState->SavePlayerResults();

    HUDWidget->GameOver(bPlayerWon);
}

void ATRPGPlayerController::OnMouseLeftClicked()
{
    if (!bIsPlayerTurn)
    {
        return;
    }

    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    ABaseUnit* ActiveUnit = GameState->GetUnitByIndex(PlayerActiveUnitIndex);
    check(ActiveUnit);

    //Get actor hit
    FHitResult HitResult;
    GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, HitResult);

    if (ActiveUnit->GetUnitState() != EUnitState::Moving || ActiveUnit->GetUnitState() != EUnitState::Combating)
    {
        if (ABaseUnit* HitUnit = Cast<ABaseUnit>(HitResult.GetActor()))
        {
            if (HitUnit->GetControllerOwner() == ControllerOwnerName && HitUnit->GetUnitIndex() != PlayerActiveUnitIndex)
            {
                SetActiveUnit(HitUnit);
            }
        }
    }
}

void ATRPGPlayerController::OnMouseRightClicked()
{
    if (!bIsPlayerTurn)
    {
        return;
    }

    UE_LOG(LogTRPGPlayerController, Display, TEXT("Right click pressed"));

    //Get actor hit
    FHitResult HitResult;
    GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, HitResult);

    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

    if (ABaseUnit* ActiveUnit = GameState->GetUnitByIndex(PlayerActiveUnitIndex))
    {
        if (ActiveUnit->GetUnitState() == EUnitState::ReadyToMove)
        {
            if (ABaseTile* DestinationTile = Cast<ABaseTile>(HitResult.GetActor()))
            {
                if (GameState->GetTerrain()->CheckAvailableTile(DestinationTile))
                {
                    TArray<FVector> TilesPath = GameState->GetTerrain()->GetPath(DestinationTile);
                    ActiveUnit->MoveUnit(TilesPath);
                    GameState->GetTerrain()->CleanAvailableTiles();
                    HUDWidget->UpdateActiveUnitData(ActiveUnit);
                }
            }
        }
        else if (ActiveUnit->GetUnitState() == EUnitState::ReadyToCombat)
        {
            UE_LOG(LogTRPGPlayerController, Display, TEXT("The Active Unit is Ready to Combat"));
            if (ABaseUnit* HitUnit = Cast<ABaseUnit>(HitResult.GetActor()))
            {
                UE_LOG(LogTRPGPlayerController, Display, TEXT("And another unit was clicked"));
                if (GameState->GetTerrain()->CheckAvailableTile(HitUnit->GetActorLocation()))
                {
                    UE_LOG(LogTRPGPlayerController, Display, TEXT("The message to Try to Use the Current Action was send to the Active Unit"));
                    ActiveUnit->TryUsingCurrentAction(HitUnit);
                    GameState->GetTerrain()->CleanAvailableTiles();
                    HUDWidget->UpdateActiveUnitData(ActiveUnit);
                }
            }
        }
    }
}

void ATRPGPlayerController::OnUnitStops(int32 PlayerUnitIndex)
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    if (ABaseUnit* Unit = GameState->GetUnitByIndex(PlayerUnitIndex))
    {
        if (PlayerUnitIndex == PlayerActiveUnitIndex)
        {
            OnMoveAction();
            HUDWidget->UpdateActiveUnitData(Unit);
        }
    }
}

void ATRPGPlayerController::OnUnitUpdateStats(ABaseUnit* Unit)
{
    HUDWidget->UpdateUnitData(Unit);

    //TODO I should take into account the case where the Unit lose dies when attack (becauso of a defenders ability)
    if (Unit->GetUnitIndex() == PlayerActiveUnitIndex)
    {
        HUDWidget->UpdateActiveUnitData(Unit);
    }
}

void ATRPGPlayerController::OnCombatAction(int32 ActionPosition)
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

    if (ABaseUnit* ActiveUnit = GameState->GetUnitByIndex(PlayerActiveUnitIndex))
    {
        ActiveUnit->SetCombatAction(ActionPosition);
        ActiveUnit->SetUnitState((EUnitState::ReadyToCombat));
        GameState->GetTerrain()->SetAvailableTiles(ActiveUnit);
    }
}

void ATRPGPlayerController::OnMoveAction()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();

    if (ABaseUnit* ActiveUnit = GameState->GetUnitByIndex(PlayerActiveUnitIndex))
    {
        ActiveUnit->SetUnitState(EUnitState::ReadyToMove);
        GameState->GetTerrain()->SetAvailableTiles(ActiveUnit);
    }
}

void ATRPGPlayerController::OnEndTurnAction()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);
    GameState->SetNextTurn();
}

void ATRPGPlayerController::OnRestartAction()
{
    ATRPGGameModeBase* GameMode = Cast<ATRPGGameModeBase>(GetWorld()->GetAuthGameMode());
    check(GameMode);
    GameMode->RestartGame();
}

void ATRPGPlayerController::OnMoveCameraAction(const FInputActionInstance& Instance)
{
    // Get the value of the Input Action for whatever type you want here
    FVector2D MoveVector = Instance.GetValue().Get<FVector2D>();

    APawn* MyPawn = GetPawn();

    if (MyPawn)
    {
        FVector CameraLocation = MyPawn->GetActorLocation();

        // Check for movement limits
        if (MoveVector.X <= -1 && CameraHMovement < -CameraHMovementMax || MoveVector.X >= 1 && CameraHMovement > CameraHMovementMax)
        {
            MoveVector.X = 0;
        }
        if (MoveVector.Y <= -1 && CameraVMovement < -CameraVMovementMax || MoveVector.Y >= 1 && CameraVMovement > CameraVMovementMax)
        {
            MoveVector.Y = 0;
        }  

        CameraHMovement += MoveVector.X;
        CameraVMovement += MoveVector.Y;

        // Move Camera
        int32 SpeedFactor = 8;
        FVector DirectionVector = MyPawn->GetActorRightVector();
        MyPawn->SetActorLocation(CameraLocation + DirectionVector * MoveVector.X * SpeedFactor + DirectionVector.RotateAngleAxis(-90, FVector::UpVector) * MoveVector.Y * SpeedFactor);
    }
    else
    {
        UE_LOG(LogTRPGPlayerController, Display, TEXT("No pawn connected to the camera was found"));
    }
}

void ATRPGPlayerController::OnZoomCameraAction(const FInputActionInstance& Instance)
{
    // Get the value of the Input Action for whatever type you want here...
    float ZoomValue = Instance.GetValue().Get<float>();

    //TODO como dije, esto lo hago aca para testear. En un futuro, deberia definir el lugar correcto en el que deberia estar la main camera,
    //PlayerState creo yo, y pasar la funcionalidad ahi, siendo llamada desde esta accion

    APawn* MyPawn = GetPawn();
    check(MyPawn);

    //TODO crear alguna limitacion para el zoom
    //TODO ver cual es el valor tan diminuto que tira para casi no mover nada
    // Zoom camera
    //MyPawn->AddMovementInput(MyPawn->GetActorForwardVector(), ZoomValue * 1000000);


    float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
    FVector DeltaLocation = FVector::ZeroVector; // The same as: FVector DeltaLocation(0.f);
    DeltaLocation.X = ZoomValue * 1000 * DeltaTime;
    MyPawn->AddActorLocalOffset(DeltaLocation, true);
}