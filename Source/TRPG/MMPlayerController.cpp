// Fill out your copyright notice in the Description page of Project Settings.


#include "MMPlayerController.h"
#include "MMHUDWidget.h"
#include "Blueprint/UserWidget.h"

AMMPlayerController::AMMPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AMMPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void AMMPlayerController::BeginPlay()
{
    Super::BeginPlay();
    HUDWidget = CreateWidget<UMMHUDWidget>(this, HUDWidgetClass);
    check(HUDWidget);
    HUDWidget->AddToViewport();
}