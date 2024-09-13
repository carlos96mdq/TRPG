// Copyright Epic Games, Inc. All Rights Reserved.


#include "TRPGGameModeBase.h"
#include "TRPGPlayerController.h"
#include "TRPGGameStateBase.h"
#include "TRPGPlayerState.h"
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