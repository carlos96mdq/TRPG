// Fill out your copyright notice in the Description page of Project Settings.


#include "TurnOrderWidget.h"
#include "TRPGGameStateBase.h"
#include "Components/Image.h"
#include "BaseUnit.h"


void UTurnOrderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//GetWorld()->GetGameState<ATRPGGameStateBase>()->OnActiveUnitSet.AddUObject(this, &UTurnOrderWidget::SetTurnOrder);
}

//void UTurnOrderWidget::SetTurnOrder(ABaseUnit* ActiveUnit)
//{
//	// If the widget is hidden, show it
//	if (GetVisibility() != ESlateVisibility::Visible)
//		SetVisibility(ESlateVisibility::Visible);
//
//	// Set all the icons in order
//	PreviousTurn->SetBrushFromTexture(GetWorld()->GetGameState<ATRPGGameStateBase>()->GetUnitByActiveUnitReference(-1)->GetIcon());
//	CurrentTurn->SetBrushFromTexture(ActiveUnit->GetIcon());
//	NextTurn->SetBrushFromTexture(GetWorld()->GetGameState<ATRPGGameStateBase>()->GetUnitByActiveUnitReference(1)->GetIcon());
//	SecondNextTurn->SetBrushFromTexture(GetWorld()->GetGameState<ATRPGGameStateBase>()->GetUnitByActiveUnitReference(2)->GetIcon());
//	ThirdNextTurn->SetBrushFromTexture(GetWorld()->GetGameState<ATRPGGameStateBase>()->GetUnitByActiveUnitReference(3)->GetIcon());
//}