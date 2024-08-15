// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NpcController.generated.h"

class ANpcUnit;

UCLASS()
class TRPG_API ANpcController : public AActor
{
	GENERATED_BODY()
	
	bool bIsNpcTurn = false;
	bool bWatingForNpcResponse = false;

	TArray<ANpcUnit*> NpcUnits;

	void RegisterAllNpcs();
	void NewTurnStarts(bool bIsPlayer);
	void CheckForNextAction();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this actor's properties
	ANpcController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnUnitStops(int32 UnitIndex);
};
