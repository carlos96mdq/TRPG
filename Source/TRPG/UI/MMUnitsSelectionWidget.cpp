// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MMUnitsSelectionWidget.h"
#include "System/MMPlayerController.h"
#include "System/MMPlayerState.h"
#include "Components/Image.h"
#include "Components/Button.h"

void UMMUnitsSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ImagesArray.Add(Image0);
	ImagesArray.Add(Image1);
	ImagesArray.Add(Image2);
	
	ButtonUnit0->OnClicked.AddUniqueDynamic(this, &UMMUnitsSelectionWidget::SelectUnit0);
	ButtonUnit1->OnClicked.AddUniqueDynamic(this, &UMMUnitsSelectionWidget::SelectUnit1);
	ButtonUnit2->OnClicked.AddUniqueDynamic(this, &UMMUnitsSelectionWidget::SelectUnit2);

	SetSelectableUnits();
}

void UMMUnitsSelectionWidget::SetSelectableUnits()
{
	SetVisibility(ESlateVisibility::Visible);

	AMMPlayerState* MyPlayerState = GetOwningPlayer<AMMPlayerController>()->GetPlayerState<AMMPlayerState>();
	check(MyPlayerState);

	int32 UnitsQuantity = MyPlayerState->GetUnitsQuantity();

	for (int32 i = 0; i < UnitsQuantity; i++)
	{
		UTexture2D* Icon = MyPlayerState->GetUnitIcon(i);
		if (Icon)
		{
			ImagesArray[i]->SetBrushFromTexture(Icon);
		}
	}
}

void UMMUnitsSelectionWidget::SelectUnit0()
{
	SelectUnit(0);
}

void UMMUnitsSelectionWidget::SelectUnit1()
{
	SelectUnit(1);
}

void UMMUnitsSelectionWidget::SelectUnit2()
{
	SelectUnit(2);
}

void UMMUnitsSelectionWidget::SelectUnit(int32 UnitIndex)
{
	AMMPlayerState* MyPlayerState = GetOwningPlayer<AMMPlayerController>()->GetPlayerState<AMMPlayerState>();
	check(MyPlayerState);

	MyPlayerState->SetSelectedUnit(UnitIndex);
}