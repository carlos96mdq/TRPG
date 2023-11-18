// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStash.h"

// Sets default values
APlayerStash::APlayerStash()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerStash::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerStash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

