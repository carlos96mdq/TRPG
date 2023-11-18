// Copyright Epic Games, Inc. All Rights Reserved.


#include "TRPGGameModeBase.h"
#include "TRPGPlayerController.h"
#include "TRPGGameStateBase.h"
#include "TRPGPlayerState.h"

ATRPGGameModeBase::ATRPGGameModeBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	GameStateClass = ATRPGGameStateBase::StaticClass();
	PlayerControllerClass = ATRPGPlayerController::StaticClass();
	PlayerStateClass = ATRPGPlayerState::StaticClass();
}