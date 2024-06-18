// Fill out your copyright notice in the Description page of Project Settings.


#include "MMUnitsSelectionWidget.h"
#include "MMPlayerController.h"
#include "MMPlayerState.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "IndexButton.h"

void UMMUnitsSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ButtonUnit0->SetIndex(0);
	ButtonUnit1->SetIndex(1);
	ButtonUnit2->SetIndex(2);

	ImagesArray.Add(Image0);
	ImagesArray.Add(Image1);
	ImagesArray.Add(Image2);
	
	ButtonUnit0->OnIndexButtonClicked.AddUObject(this, &UMMUnitsSelectionWidget::SelectUnit);
	ButtonUnit1->OnIndexButtonClicked.AddUObject(this, &UMMUnitsSelectionWidget::SelectUnit);
	ButtonUnit2->OnIndexButtonClicked.AddUObject(this, &UMMUnitsSelectionWidget::SelectUnit);

	GetOwningPlayer<AMMPlayerController>()->GetPlayerState<AMMPlayerState>()->OnSelectableUnitsGet.AddUObject(this, &UMMUnitsSelectionWidget::SetSelectableUnits);
}

//TODO puede que lo mejor sea setearlo con multiples parametros, o mandarle el array con los FName, pero me parecio mejor esta conversacion de ida y vuelta
void UMMUnitsSelectionWidget::SetSelectableUnits()
{
	// If the widget is hidden, show it
	if (GetVisibility() != ESlateVisibility::Visible)
		SetVisibility(ESlateVisibility::Visible);

	AMMPlayerState* MyPlayerState = GetOwningPlayer<AMMPlayerController>()->GetPlayerState<AMMPlayerState>();
	check(MyPlayerState);

	int32 UnitsQuantity = MyPlayerState->GetUnitsQuantity();

	for (size_t i = 0; i < UnitsQuantity; i++)
	{
		UTexture2D* Icon = MyPlayerState->GetUnitIcon(i);
		if (Icon)
		{
			ImagesArray[i]->SetBrushFromTexture(Icon);
		}
	}
}

void UMMUnitsSelectionWidget::SelectUnit(int32 UnitIndex)
{
	AMMPlayerState* MyPlayerState = GetOwningPlayer<AMMPlayerController>()->GetPlayerState<AMMPlayerState>();
	check(MyPlayerState);

	MyPlayerState->SetSelectedUnit(UnitIndex);
}