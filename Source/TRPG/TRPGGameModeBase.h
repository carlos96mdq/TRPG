// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TRPGGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TRPG_API ATRPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	ATRPGGameModeBase(const FObjectInitializer& ObjectInitializer);

public:
	void RestartGame();
};
