// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UnitDataIcon.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UUnitDataIcon::SetData(const FUnitDataParams& UnitDataParams)
{
	UnitIcon->SetBrushFromTexture(UnitDataParams.Texture2D);
	UnitArmor->SetText(FText::AsNumber(UnitDataParams.Armor));
	UnitLife->SetText(FText::AsNumber(UnitDataParams.Life));
	UnitEnergy->SetText(FText::AsNumber(UnitDataParams.Energy));
}