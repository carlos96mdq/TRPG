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
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Camera/CameraActor.h" //TODO al mover a la mainCamera, mover este include con ella

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

    //HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
    //check(HUDWidget);
    //HUDWidget->AddToViewport();
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
    
    //HUDWidget = CreateWidget<UHUDWidget>(this, HUDWidgetClass);
    //check(HUDWidget);
    //HUDWidget->AddToViewport();
    MainCamera = GetWorld()->SpawnActor<ACameraActor>(FVector(200.0f, 500.0f, 600.0f), FRotator(-60.0f, -45.0f, 0.0f));
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
        MyPlayerState->ChangeState(EUnitState::ReadyToCombat, ActionPosition);
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
        MyPlayerState->ChangeState(EUnitState::ReadyToMove);
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
        MyPlayerState->ChangeState(EUnitState::Idle);
        GetWorld()->GetGameState<ATRPGGameStateBase>()->SetNextTurn();
    }
    else
    {
        checkNoEntry();
    }
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
    UE_LOG(LogTemp, Warning, TEXT("Scroll value: %f"), ZoomValue);
    //MyPawn->AddMovementInput(MyPawn->GetActorForwardVector(), ZoomValue * 1000000);


    float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);
    FVector DeltaLocation = FVector::ZeroVector; // The same as: FVector DeltaLocation(0.f);
    DeltaLocation.X = ZoomValue * 1000 * DeltaTime;
    MyPawn->AddActorLocalOffset(DeltaLocation, true);
}