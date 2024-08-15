// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionWidget.h"
#include "Components/Button.h"
#include "TRPGPlayerController.h"

void UActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonCombat->OnClicked.AddUniqueDynamic(this, &UActionWidget::CombatPressed);
	ButtonMove->OnClicked.AddUniqueDynamic(this, &UActionWidget::MovePressed);
	ButtonEndTurn->OnClicked.AddUniqueDynamic(this, &UActionWidget::EndTurnPressed);
}

void UActionWidget::CombatPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Combat button pressed"));
	// TODO en esta parte en realidad debería darse la orden de abrir un submenu, elegir el ataque o habilidad a usar y luego si hacer la invocación de abajo para usar esa CombatAction
	GetOwningPlayer<ATRPGPlayerController>()->OnCombatAction(0);
}

void UActionWidget::MovePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Move button pressed"));
	GetOwningPlayer<ATRPGPlayerController>()->OnMoveAction();
}

void UActionWidget::EndTurnPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Wait button pressed"));
	GetOwningPlayer<ATRPGPlayerController>()->OnEndTurnAction();
}

