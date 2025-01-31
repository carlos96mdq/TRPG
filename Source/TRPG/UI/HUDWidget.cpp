// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "System/TRPGGameStateBase.h"
#include "System/TRPGPlayerController.h"
#include "UI/ActiveUnitWidget.h"
#include "UI/SelectedUnitWidget.h"
#include "UI/UnitDataIcon.h"
#include "Units/BaseUnit.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ActiveUnitWidget->SetVisibility(ESlateVisibility::Collapsed);
	GameOverScreen->SetVisibility(ESlateVisibility::Collapsed);
	//LoadScreen->SetVisibility(ESlateVisibility::Visible);
}

void UHUDWidget::RestartPressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnRestartAction();
}

void UHUDWidget::SetPlayerTurn(bool bIsPlayerTurn)
{
	if (bIsPlayerTurn)
	{
		ActiveUnitWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ActiveUnitWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UHUDWidget::CreateUnitDataWidget(ABaseUnit* Unit, const TSubclassOf<UUnitDataIcon>& UnitDataIconClass, bool bBelongsToPlayer)
{
	int32 UnitIndex = Unit->GetUnitIndex();
	
	TMap<int32, UUnitDataIcon*>& DataIconList = bBelongsToPlayer ? UnitDataIconList : EnemyDataIconList;

	UUnitDataIcon* DataIcon = DataIconList.Emplace(UnitIndex, CreateWidget<UUnitDataIcon>(this, UnitDataIconClass));
	
	if (bBelongsToPlayer)
	{ 
		PlayerUnits->AddChild(DataIcon);
	}
	else
	{
		EnemyUnits->AddChild(DataIcon);
	}

	DataIcon->SetData(FUnitDataParams(Unit->GetIcon(), Unit->GetArmor(), Unit->GetLife(), Unit->GetEnergy()));
}

void UHUDWidget::UpdateUnitData(ABaseUnit* Unit)
{
	UUnitDataIcon** UnitDataIconTemp = UnitDataIconList.Contains(Unit->GetUnitIndex()) ? UnitDataIconList.Find(Unit->GetUnitIndex()) : EnemyDataIconList.Find(Unit->GetUnitIndex());
	UUnitDataIcon* UnitDataIcon = *UnitDataIconTemp;

	if (UnitDataIcon)
	{
		if (Unit->IsAlive())
		{
			UnitDataIcon->SetData(FUnitDataParams(Unit->GetIcon(), Unit->GetArmor(), Unit->GetLife(), Unit->GetEnergy()));
		}
		else
		{
			// This use a placeholder just for reference. In the future a more generic method to get customizable images should be implemented
			UTexture2D* DefeatTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/TRPG/Textures2D/IconDefeat.IconDefeat"));
			if (DefeatTexture)
			{
				UnitDataIcon->SetData(FUnitDataParams(DefeatTexture, 0, 0, 0));
			}
		}
	}
}

void UHUDWidget::UpdateActiveUnitData(ABaseUnit* Unit)
{ 
	ActiveUnitWidget->UpdateUnitData(Unit);
}

void UHUDWidget::GameOver(bool bPlayerWon)
{
	FText CustomText;

	if (bPlayerWon)
	{
		CustomText = NSLOCTEXT("GameOver", "Win", "You Win!");
	}
	else
	{
		CustomText = NSLOCTEXT("GameOver", "Lose", "You Lose!");
	}

	// To test other way, I created a function in blueprint and in this code we access that function
	FName FunctionName(TEXT("SetGameOverText"));
	UFunction* SetTextFunction = FindFunction(FunctionName);

	if (SetTextFunction)
	{
		struct
		{
			FText NewText;
		} Params;
		Params.NewText = CustomText;

		// Call the function in Blueprint to update the text
		ProcessEvent(SetTextFunction, &Params);
	}

	GameOverScreen->SetVisibility(ESlateVisibility::Visible);
}