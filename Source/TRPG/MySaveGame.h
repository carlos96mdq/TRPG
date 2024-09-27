// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TRPG_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MatchesPlayed = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MatchesWon = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MatchesLost = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 UnitsDefeated = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 UnitsLost = 0;
};
