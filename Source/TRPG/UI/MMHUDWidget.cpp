// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MMHUDWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "System/MySaveGame.h"

void UMMHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	LoadScreen->SetVisibility(ESlateVisibility::Collapsed);
	PlayButton->OnClicked.AddUniqueDynamic(this, &UMMHUDWidget::PlayPressed);
}

void UMMHUDWidget::PlayPressed()
{
	// Clean screen
	PlayButton->RemoveFromParent();

	// Show loading screen
	LoadScreen->SetVisibility(ESlateVisibility::Visible);

	// Starts the new level
	// TODO: This should be done in PlayerController
	UGameplayStatics::OpenLevel(GetWorld(), FName("Main"), true);
	//UGameplayStatics::LoadStreamLevel(GetWorld(), FName("Main"), true, true, FLatentActionInfo());
}

void UMMHUDWidget::SetPlayerMatchesRecordData(UMySaveGame* LoadedData)
{
	if (LoadedData)
	{
		MatchesPlayedValue->SetText(FText::AsNumber(LoadedData->MatchesPlayed));
		MatchesWonValue->SetText(FText::AsNumber(LoadedData->MatchesWon));
		MatchesLostValue->SetText(FText::AsNumber(LoadedData->MatchesLost));
		UnitsDefeatedValue->SetText(FText::AsNumber(LoadedData->UnitsDefeated));
		UnitsLostValue->SetText(FText::AsNumber(LoadedData->UnitsLost));
	}
}