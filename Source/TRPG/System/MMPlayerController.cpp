// Fill out your copyright notice in the Description page of Project Settings.


#include "System/MMPlayerController.h"
#include "UI/MMHUDWidget.h"
#include "Blueprint/UserWidget.h"

AMMPlayerController::AMMPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AMMPlayerController::ShowPlayerMatchesRecordData(const TArray<int32>& PlayerMatchesRecordData)
{
    HUDWidget = CreateWidget<UMMHUDWidget>(this, HUDWidgetClass);
    HUDWidget->AddToViewport();
    HUDWidget->SetPlayerMatchesRecordData(PlayerMatchesRecordData);
}