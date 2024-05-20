// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "TRPGGameStateBase.h"
#include "ActiveUnitWidget.h"
#include "SelectedUnitWidget.h"
#include "TurnOrderWidget.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ActiveUnitWidget->SetVisibility(ESlateVisibility::Collapsed);
	SelectedUnitWidget->SetVisibility(ESlateVisibility::Collapsed);
	TurnOrderWidget->SetVisibility(ESlateVisibility::Collapsed);
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
//	// TODO: Esto lo handleo directamente ac� para hacer pruebas. Lo ideal ser�a que esto vaya al PlayerController y este se encargara de mandar la orden
//	// TODO: Lo dejo de muestra, pero como no tengo mas el boton Play esto ya no va m�s
//	GetWorld()->GetGameState<ATRPGGameStateBase>()->StartGame();
//}