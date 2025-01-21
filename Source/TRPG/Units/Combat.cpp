// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Combat.h"

// Sets default values
ACombat::ACombat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACombat::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACombat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

