// Copyright Epic Games, Inc. All Rights Reserved.


#include "System/TRPGGameModeBase.h"
#include "System/TRPGPlayerController.h"
#include "System/TRPGGameStateBase.h"
#include "System/TRPGPlayerState.h"
#include "Kismet/GameplayStatics.h"

ATRPGGameModeBase::ATRPGGameModeBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	GameStateClass = ATRPGGameStateBase::StaticClass();
	PlayerControllerClass = ATRPGPlayerController::StaticClass();
	PlayerStateClass = ATRPGPlayerState::StaticClass();
}

void ATRPGGameModeBase::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}