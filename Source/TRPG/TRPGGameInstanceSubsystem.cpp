// Fill out your copyright notice in the Description page of Project Settings.


#include "TRPGGameInstanceSubsystem.h"

void UTRPGGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UTRPGGameInstanceSubsystem::SetSelectedUnitName(FName NewName)
{
	SelectedUnitName = NewName;
}