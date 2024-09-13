// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NpcController.generated.h"

class ANpcUnit;
enum class EUnitControllerOwner : uint8;

UCLASS()
class TRPG_API ANpcController : public AActor
{
	GENERATED_BODY()
	
	bool bIsNpcTurn = false;
	bool bCheckForNextAction = false;
	EUnitControllerOwner ControllerOwnerName;

	TArray<ANpcUnit*> NpcUnits;

	void RegisterAllNpcs();
	void NewTurnStarts(EUnitControllerOwner ControllerTurn);
	void CheckForNextAction();
	void GameOver(EUnitControllerOwner WinnerController);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	ANpcController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnUnitStops(int32 NpcUnitIndex);
	void OnUnitUpdateStats(int32 NpcUnitIndex);
};
