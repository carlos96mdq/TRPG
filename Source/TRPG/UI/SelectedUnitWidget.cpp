// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SelectedUnitWidget.h"
#include "System/TRPGPlayerState.h"
#include "Components/TextBlock.h"
#include "Units/BaseUnit.h"


void USelectedUnitWidget::NativeConstruct()
{
	UE_LOG(LogTemp, Warning, TEXT("UActiveUnitWidget::NativeConstruct"));
	Super::NativeConstruct();

	GetOwningPlayerState<ATRPGPlayerState>()->OnUnitSelected.AddUObject(this, &USelectedUnitWidget::SetUnitData);
}

void USelectedUnitWidget::SetUnitData(ABaseUnit* SelectedUnit)
{
	// Get Active Unit and set widget data
	//if (ABaseUnit* SelectedUnit = GetOwningPlayerState<ATRPGPlayerState>()->GetSelectedUnit())
	if (SelectedUnit)
	{
		LabelName->SetText(FText::FromName(SelectedUnit->GetUnitName()));
		SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
}