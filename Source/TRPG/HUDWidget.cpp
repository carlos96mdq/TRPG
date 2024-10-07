// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "TRPGGameStateBase.h"
#include "TRPGPlayerController.h"
#include "ActiveUnitWidget.h"
#include "SelectedUnitWidget.h"
#include "UnitDataIcon.h"
#include "BaseUnit.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ActiveUnitWidget->SetVisibility(ESlateVisibility::Collapsed);
	GameOverScreen->SetVisibility(ESlateVisibility::Collapsed);
	//LoadScreen->SetVisibility(ESlateVisibility::Visible);

	//PlayButton->OnClicked.AddUniqueDynamic(this, &UHUDWidget::PlayPressed);
}

//void UHUDWidget::PlayPressed()
//{
//	// Close Load Screen
//	PlayButton->SetVisibility(ESlateVisibility::Collapsed);
//	LoadScreen->SetVisibility(ESlateVisibility::Collapsed);
//	PlayButton->RemoveFromParent();
//	LoadScreen->RemoveFromParent();
//
//	// Starts the turn system
//	// TODO: Esto lo handleo directamente acá para hacer pruebas. Lo ideal sería que esto vaya al PlayerController y este se encargara de mandar la orden
//	// TODO: Lo dejo de muestra, pero como no tengo mas el boton Play esto ya no va más
//	GetWorld()->GetGameState<ATRPGGameStateBase>()->StartGame();
//}

void UHUDWidget::RestartPressed()
{
	GetOwningPlayer<ATRPGPlayerController>()->OnRestartAction();
}


void UHUDWidget::SetPlayerTurn(bool bIsPlayerTurn)
{
	if (bIsPlayerTurn)
		ActiveUnitWidget->SetVisibility(ESlateVisibility::Visible);
	else
		ActiveUnitWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UHUDWidget::CreateUnitDataWidget(ABaseUnit* Unit, const TSubclassOf<UUnitDataIcon>& UnitDataIconClass, bool IsThisPlayer)
{
	int32 UnitIndex = Unit->GetUnitIndex();
	
	TMap<int32, UUnitDataIcon*>& DataIconList = IsThisPlayer ? UnitDataIconList : EnemyDataIconList;

	UUnitDataIcon* DataIcon = DataIconList.Emplace(UnitIndex, CreateWidget<UUnitDataIcon>(this, UnitDataIconClass));
	
	if (IsThisPlayer)
		PlayerUnits->AddChild(DataIcon);
	else
		EnemyUnits->AddChild(DataIcon);

	DataIcon->UnitIcon->SetBrushFromTexture(Unit->GetIcon());
	DataIcon->UnitArmor->SetText(FText::AsNumber(Unit->GetArmor()));
	DataIcon->UnitLife->SetText(FText::AsNumber(Unit->GetLife()));
	DataIcon->UnitEnergy->SetText(FText::AsNumber(Unit->GetEnergy()));
}

void UHUDWidget::UpdateUnitData(ABaseUnit* Unit)
{
	UUnitDataIcon** UnitDataIconTemp = UnitDataIconList.Contains(Unit->GetUnitIndex()) ? UnitDataIconList.Find(Unit->GetUnitIndex()) : EnemyDataIconList.Find(Unit->GetUnitIndex());
	UUnitDataIcon* UnitDataIcon = *UnitDataIconTemp;

	if (UnitDataIcon)
	{
		if (Unit->IsAlive())
		{
			UnitDataIcon->UnitIcon->SetBrushFromTexture(Unit->GetIcon());
			UnitDataIcon->UnitArmor->SetText(FText::AsNumber(Unit->GetArmor()));
			UnitDataIcon->UnitLife->SetText(FText::AsNumber(Unit->GetLife()));
			UnitDataIcon->UnitEnergy->SetText(FText::AsNumber(Unit->GetEnergy()));
		}
		else
		{
			// This use a placeholder just for reference. In the future a more generic method to get customizable images should be implemented
			UTexture2D* DefeatTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/TRPG/Textures2D/IconDefeat.IconDefeat"));
			if (DefeatTexture)
				UnitDataIcon->UnitIcon->SetBrushFromTexture(DefeatTexture);

			UnitDataIcon->UnitArmor->SetText(FText::AsNumber(0));
			UnitDataIcon->UnitLife->SetText(FText::AsNumber(0));
			UnitDataIcon->UnitEnergy->SetText(FText::AsNumber(0));
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
		CustomText = FText::FromString("You Win!");
	else
		CustomText = FText::FromString("You Lose!");

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