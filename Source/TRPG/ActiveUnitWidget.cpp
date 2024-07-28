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
	ButtonMove->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::MovePressed);
	ButtonWait->OnClicked.AddUniqueDynamic(this, &UActiveUnitWidget::WaitPressed);

	CombatButtons.Add(ButtonCombat0);
	CombatButtons.Add(ButtonCombat1);
	CombatButtons.Add(ButtonCombat2);
	CombatButtons.Add(ButtonCombat3);

	//GetWorld()->GetGameState<ATRPGGameStateBase>()->OnActiveUnitSet.AddUObject(this, &UActiveUnitWidget::SetUnitData);
	//GetOwningPlayer<ATRPGPlayerController>()->OnActiveUnitSet.AddUObject(this, &UActiveUnitWidget::UpdateUnitData);
	//GetOwningPlayerState<ATRPGPlayerState>()->OnStateChanged.AddUObject(this, &UActiveUnitWidget::ChangeButtonsState);
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

void UActiveUnitWidget::MovePressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnMoveAction();
}

void UActiveUnitWidget::WaitPressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnWaitAction();
}

void UActiveUnitWidget::UpdateUnitData(ABaseUnit* ActiveUnit)
{
	LabelName->SetText(FText::FromName(ActiveUnit->GetName()));

	if (GetVisibility() != ESlateVisibility::Visible)
		SetVisibility(ESlateVisibility::Visible);

	if (ActiveUnit->GetUnitState() == EUnitState::Moving || ActiveUnit->GetUnitState() == EUnitState::Combating)
	{
		ButtonMove->SetIsEnabled(false);
		ButtonWait->SetIsEnabled(false);

		for (UButton* Button : CombatButtons)
			Button->SetIsEnabled(false);
	}
	else
	{
		int32 UnitEnergy = ActiveUnit->GetEnergy();

		ButtonWait->SetIsEnabled(true);
		//TODO tambien antes que otra cosa se deberian setear todas las imagenes

		if (UnitEnergy <= 0)
			ButtonMove->SetIsEnabled(false);
		else
			ButtonMove->SetIsEnabled(true);

		for (size_t i = 0; i < CombatButtons.Num(); i++)
		{
			UButton* CombatButton = CombatButtons[i];

			if (ActiveUnit->HasActionEquipped(i) && ActiveUnit->GetCombatActionEnergyCost(i) <= UnitEnergy)
				CombatButton->SetIsEnabled(true);
			else
				CombatButton->SetIsEnabled(false);
		}
	}
}

void UActiveUnitWidget::ChangeButtonsState(EUnitState NewState)
{	
	//TODO deberia agarrar los botones y agregarlos en run time de igual manera que agrego los UnitDataIcon
	if (NewState == EUnitState::Idle || NewState == EUnitState::ReadyToMove || NewState == EUnitState::ReadyToCombat)
	{

	}

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
		break;
	default:
		break;
	}
}