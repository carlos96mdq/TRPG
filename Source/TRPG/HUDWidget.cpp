// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "TRPGGameStateBase.h"
#include "TRPGPlayerController.h"
#include "ActiveUnitWidget.h"
#include "SelectedUnitWidget.h"
#include "TurnOrderWidget.h"

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
	{
		ActiveUnitWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		ActiveUnitWidget->SetVisibility(ESlateVisibility::Collapsed);
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