// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/NpcUnit.h"

void ANpcUnit::TurnStarts()
{
	Super::TurnStarts();

	SetAiState(EAiState::WaitingForOrders);
}

void ANpcUnit::SetAiState(EAiState NewState)
{
	AiState = NewState;
}

void ANpcUnit::SetCurrentObjective(ABaseUnit* NewObjective)
{
	CurrentObjective = NewObjective;
}