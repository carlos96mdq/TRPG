// Fill out your copyright notice in the Description page of Project Settings.


#include "MMHUDWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

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
	// TODO: Esto lo handleo directamente acá para hacer pruebas. Lo ideal sería que esto vaya al PlayerController y este se encargara de mandar la orden
	UGameplayStatics::OpenLevel(GetWorld(), FName("Main"), true);
	//UGameplayStatics::LoadStreamLevel(GetWorld(), FName("Main"), true, true, FLatentActionInfo());
}