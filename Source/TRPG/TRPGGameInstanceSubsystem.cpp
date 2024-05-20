// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGGameInstanceSubsystem.h"

void UTRPGGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UE_LOG(LogTemp, Warning, TEXT("Inicializando UTRPGGameInstanceSubsystem"));
}

void UTRPGGameInstanceSubsystem::SetSelectedUnitName(FName NewName)
{
	SelectedUnitName = NewName;
}