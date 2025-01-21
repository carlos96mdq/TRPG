// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/BaseUnit.h"
#include "NpcUnit.generated.h"

UENUM()
enum class EAiState : uint8 {
	WaitingForOrders,
	Sleeping
};

UENUM()
enum class EAiBehaviour : uint8 {
	Normal,
	Aggresive,
	Defensive
};

/**
 * 
 */
UCLASS()
class TRPG_API ANpcUnit : public ABaseUnit
{
	GENERATED_BODY()

	EAiState AiState = EAiState::WaitingForOrders;
	EAiBehaviour AiMode = EAiBehaviour::Normal;

public:

	void TurnStarts() override;

	void SetAiState(EAiState NewState);
	void SetCurrentObjective(ABaseUnit* NewObjective);

	EAiState GetAiState() { return AiState; }
	ABaseUnit* GetCurrentObjective() { return CurrentObjective; }

};
