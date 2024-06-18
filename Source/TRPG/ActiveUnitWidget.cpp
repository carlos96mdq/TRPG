// Fill out your copyright notice in the Description page of Project Settings.


#include "ActiveUnitWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TRPGPlayerController.h"
#include "TRPGGameStateBase.h"
#include "TRPGPlayerState.h"
#include "BaseUnit.h"

void UActiveUnitWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonCombat0->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::Combat0Pressed);
	ButtonCombat1->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::Combat1Pressed);
	ButtonCombat2->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::Combat2Pressed);
	ButtonCombat3->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::Combat3Pressed);
	ButtonCombat4->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::Combat4Pressed);
	ButtonMove->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::MovePressed);
	ButtonWait->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::WaitPressed);

	GetWorld()->GetGameState<ATRPGGameStateBase>()->OnActiveUnitSet.AddUObject(this, &UActiveUnitWidget::SetUnitData);
	GetOwningPlayerState<ATRPGPlayerState>()->OnStateChanged.AddUObject(this, &UActiveUnitWidget::ChangeButtonsState);
}

void UActiveUnitWidget::Combat0Pressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnCombatAction(0);
}

void UActiveUnitWidget::Combat1Pressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnCombatAction(1);
}

void UActiveUnitWidget::Combat2Pressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnCombatAction(2);
}

void UActiveUnitWidget::Combat3Pressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnCombatAction(3);
}

void UActiveUnitWidget::Combat4Pressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnCombatAction(4);
}

void UActiveUnitWidget::MovePressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnMoveAction();
}

void UActiveUnitWidget::WaitPressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnWaitAction();
}

void UActiveUnitWidget::SetUnitData(ABaseUnit* ActiveUnit)
{
	LabelName->SetText(FText::FromName(ActiveUnit->GetName()));
	LabelArchetype->SetText(FText::FromName(ActiveUnit->GetArchetype()));

	// If the widget is hidden, show it
	if (GetVisibility() != ESlateVisibility::Visible)
		SetVisibility(ESlateVisibility::Visible);

	// Enable anly buttons that have actions handled
	ButtonMove->SetIsEnabled(true);
	if (ActiveUnit->HasActionEquipped(0))
		ButtonCombat0->SetIsEnabled(true);
	if (ActiveUnit->HasActionEquipped(1))
		ButtonCombat1->SetIsEnabled(true);
	if (ActiveUnit->HasActionEquipped(2))
		ButtonCombat2->SetIsEnabled(true);
	if (ActiveUnit->HasActionEquipped(3))
		ButtonCombat3->SetIsEnabled(true);
	if (ActiveUnit->HasActionEquipped(4))
		ButtonCombat4->SetIsEnabled(true);
}

void UActiveUnitWidget::ChangeButtonsState(EUnitState NewState)
{
	// Disable buttons according to state
	switch (NewState)
	{
	case EUnitState::Idle:
		break;
	case EUnitState::ReadyToMove:
		break;
	case EUnitState::Moving:
		ButtonMove->SetIsEnabled(false);
		break;
	case EUnitState::ReadyToCombat:
		break;
	case EUnitState::Combating:
		ButtonCombat0->SetIsEnabled(false);
		ButtonCombat1->SetIsEnabled(false);
		ButtonCombat2->SetIsEnabled(false);
		ButtonCombat3->SetIsEnabled(false);
		ButtonCombat4->SetIsEnabled(false);
		break;
	default:
		break;
	}
}