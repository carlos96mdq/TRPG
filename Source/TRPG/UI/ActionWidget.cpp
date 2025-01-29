// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ActionWidget.h"
#include "Components/Button.h"
#include "System/TRPGPlayerController.h"

void UActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonCombat->OnClicked.AddUniqueDynamic(this, &UActionWidget::CombatPressed);
	ButtonMove->OnClicked.AddUniqueDynamic(this, &UActionWidget::MovePressed);
	ButtonEndTurn->OnClicked.AddUniqueDynamic(this, &UActionWidget::EndTurnPressed);
}

void UActionWidget::CombatPressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnCombatAction(0);
}

void UActionWidget::MovePressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnMoveAction();
}

void UActionWidget::EndTurnPressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnEndTurnAction();
}

