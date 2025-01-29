// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TRPGGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class TRPG_API UTRPGGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	FName SelectedUnitName = "Default";

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override {};
	virtual void Deinitialize() override {};

	void SetSelectedUnitName(FName NewName);

	FName GetSelectedUnitName() const { return SelectedUnitName; }
};
