// Fill out your copyright notice in the Description page of Project Settings.


#include "MMGameModeBase.h"
#include "MMPlayerController.h"

AMMGameModeBase::AMMGameModeBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	//TODO esto realmente no es necesario, ya que lo seteo como un BP en el editor. Lo dejo como referencia a futuro
	PlayerControllerClass = AMMPlayerController::StaticClass();
}