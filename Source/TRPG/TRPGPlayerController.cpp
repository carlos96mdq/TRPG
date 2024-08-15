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
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraActor.h" //TODO al mover a la mainCamera, mover este include con ella
#include "UnitDataIcon.h"

ATRPGPlayerController::ATRPGPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    // Spawn this here, so the camera appears in the editor level so it can be positioned there
    /*MainCamera = GetWorld()->SpawnActor<ACameraActor>();*/
}

void ATRPGPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    GetWorld()->GetGameState<ATRPGGameStateBase>()->OnGameStarts.AddUObject(this, &ATRPGPlayerController::CreateUnitsData);
    GetWorld()->GetGameState<ATRPGGameStateBase>()->OnNewTurnStarts.AddUObject(this, &ATRPGPlayerController::NewTurnStarts);
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
    
    MainCamera = GetWorld()->SpawnActor<ACameraActor>(FVector(200.0f, 500.0f, 600.0f), FRotator(-60.0f, -45.0f, 0.0f));
}

void ATRPGPlayerController::SetActiveUnit(ABaseUnit* NewActiveUnit)
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);
    ABaseUnit* ActiveUnit = PlayerUnits[PlayerActiveUnitIndex];
    check(ActiveUnit);

    ActiveUnit->SetUnitState(EUnitState::Idle);
    GameState->GetTerrain()->CleanAvailableTiles();

    PlayerActiveUnitIndex = NewActiveUnit->GetUnitPlayerIndex();
    OnMoveAction();

    HUDWidget->UpdateActiveUnitData(NewActiveUnit);
    //OnActiveUnitSet.Broadcast(NewActiveUnit);
}

void ATRPGPlayerController::CreateUnitsData()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);
    int32 UnitIndex = 0;

    for (int32 i = 0; i < GameState->GetUnitsQuantity(); i++)
    {
        ABaseUnit* Unit = GameState->GetUnitByIndex(i);
        check(Unit);

        // Only if the Unit is a Player's one, create the UnitDataIconWidget element, add it to the HUD in viewport and set the unit data in it
        // Also, register the unit's index to be called later and relate it to the interface
        if (Unit->IsPlayer())
        {
            PlayerUnits.Emplace(Unit);
            Unit->SetUnitPlayerIndex(UnitIndex);

            UnitDataIconList.Emplace(CreateWidget<UUnitDataIcon>(this, UnitDataIconClass));
            check(UnitDataIconList[UnitIndex]);
            HUDWidget->PlayerUnits->AddChild(UnitDataIconList[UnitIndex]);

            UnitDataIconList[UnitIndex]->UnitIcon->SetBrushFromTexture(Unit->GetIcon());
            UnitDataIconList[UnitIndex]->UnitArmor->SetText(FText::AsNumber(Unit->GetArmor()));
            UnitDataIconList[UnitIndex]->UnitLife->SetText(FText::AsNumber(Unit->GetLife()));
            UnitDataIconList[UnitIndex]->UnitEnergy->SetText(FText::AsNumber(Unit->GetEnergy()));

            Unit->OnUnitStopsMoving.AddUObject(this, &ATRPGPlayerController::OnUnitStops);
            Unit->OnUnitStopsAction.AddUObject(this, &ATRPGPlayerController::OnUnitStops);
            Unit->OnUnitUpdateStats.AddUObject(this, &ATRPGPlayerController::OnUnitUpdateStats);

            UnitIndex++;
        }
    }
}

void ATRPGPlayerController::NewTurnStarts(bool bIsPlayer)
{
    bIsPlayerTurn = bIsPlayer;
    HUDWidget->SetPlayerTurn(bIsPlayer);

    if (bIsPlayerTurn)
    {
        //TODO en realidad en esta parte deberia iterar entre todas las unidades ahsta encotnrar la primera no muerta
        SetActiveUnit(PlayerUnits[0]);
    }
    else
    {
        ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
        check(GameState);
        GameState->GetTerrain()->CleanAvailableTiles();
    }
}

void ATRPGPlayerController::OnMouseLeftClicked()
{
    if (!bIsPlayerTurn)
        return;

    ABaseUnit* ActiveUnit = PlayerUnits[PlayerActiveUnitIndex];
    check(ActiveUnit);

    //Get actor hit
    FHitResult HitResult;
    GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, HitResult);

    if (ActiveUnit->GetUnitState() != EUnitState::Moving || ActiveUnit->GetUnitState() != EUnitState::Combating)
    {
        if (ABaseUnit* HitUnit = Cast<ABaseUnit>(HitResult.GetActor()))
        {
            if (HitUnit->IsPlayer() && HitUnit->GetUnitPlayerIndex() != PlayerActiveUnitIndex)
                SetActiveUnit(HitUnit);
        }
    }
}

void ATRPGPlayerController::OnMouseRightClicked()
{
    if (!bIsPlayerTurn)
        return;

    UE_LOG(LogTemp, Display, TEXT("[PLAYER CONTROLLER] Right click pressed"));

    //Get actor hit
    FHitResult HitResult;
    GetHitResultUnderCursor(ECollisionChannel::ECC_WorldDynamic, false, HitResult);

    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);
    ABaseUnit* ActiveUnit = PlayerUnits[PlayerActiveUnitIndex];
    check(ActiveUnit);

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
        UE_LOG(LogTemp, Display, TEXT("[PLAYER CONTROLLER] The Active Unit is Ready to Combat"));
        if (ABaseUnit* HitUnit = Cast<ABaseUnit>(HitResult.GetActor()))
        {
            UE_LOG(LogTemp, Display, TEXT("[PLAYER CONTROLLER] And another unit was clicked"));
            if (GameState->GetTerrain()->CheckAvailableTile(HitUnit->GetActorLocation()))
            {
                UE_LOG(LogTemp, Display, TEXT("[PLAYER CONTROLLER] The message to Try to Use the Current Action was send to the Active Unit"));
                ActiveUnit->TryUsingCurrentAction(HitUnit);
                GameState->GetTerrain()->CleanAvailableTiles();
                HUDWidget->UpdateActiveUnitData(ActiveUnit);
            }
        }
    }
}

void ATRPGPlayerController::OnUnitStops(int32 PlayerUnitIndex)
{
    ABaseUnit* Unit = PlayerUnits[PlayerUnitIndex];
    check(Unit);

    if (PlayerUnitIndex == PlayerActiveUnitIndex)
    {
        OnMoveAction();
        HUDWidget->UpdateActiveUnitData(Unit);
    }
}

void ATRPGPlayerController::OnUnitUpdateStats(int32 PlayerUnitIndex)
{
    ABaseUnit* Unit = PlayerUnits[PlayerUnitIndex];
    check(Unit);

    UnitDataIconList[PlayerUnitIndex]->UnitIcon->SetBrushFromTexture(Unit->GetIcon());
    UnitDataIconList[PlayerUnitIndex]->UnitArmor->SetText(FText::AsNumber(Unit->GetArmor()));
    UnitDataIconList[PlayerUnitIndex]->UnitLife->SetText(FText::AsNumber(Unit->GetLife()));
    UnitDataIconList[PlayerUnitIndex]->UnitEnergy->SetText(FText::AsNumber(Unit->GetEnergy()));

    if (PlayerUnitIndex == PlayerActiveUnitIndex)
        HUDWidget->UpdateActiveUnitData(Unit);
}

void ATRPGPlayerController::OnCombatAction(int32 ActionPosition)
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);
    ABaseUnit* ActiveUnit = PlayerUnits[PlayerActiveUnitIndex];
    check(ActiveUnit);

    ActiveUnit->SetCombatAction(ActionPosition);
    ActiveUnit->SetUnitState((EUnitState::ReadyToCombat));
    GameState->GetTerrain()->SetAvailableTiles(ActiveUnit);
}

void ATRPGPlayerController::OnMoveAction()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);
    ABaseUnit* ActiveUnit = PlayerUnits[PlayerActiveUnitIndex];
    check(ActiveUnit);

    ActiveUnit->SetUnitState(EUnitState::ReadyToMove);
    GameState->GetTerrain()->SetAvailableTiles(ActiveUnit);
}

void ATRPGPlayerController::OnEndTurnAction()
{
    ATRPGGameStateBase* GameState = GetWorld()->GetGameState<ATRPGGameStateBase>();
    check(GameState);
    GameState->SetNextTurn();
}

void ATRPGPlayerController::OnMoveCameraAction(const FInputActionInstance& Instance)
{
    // Get the value of the Input Action for whatever type you want here...
    FVector2D MoveVector = Instance.GetValue().Get<FVector2D>();

    APawn* MyPawn = GetPawn();
    check(MyPawn);

    // Move the camera in the X direction
    FVector DirectionVector = MyPawn->GetActorRightVector();
    MyPawn->AddMovementInput(DirectionVector, MoveVector.X);

    // Move camera in the Y direction
    DirectionVector = DirectionVector.RotateAngleAxis(-90, FVector::UpVector);
    MyPawn->AddMovementInput(DirectionVector, MoveVector.Y);
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