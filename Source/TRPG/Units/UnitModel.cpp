// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/UnitModel.h"

// Sets default values
AUnitModel::AUnitModel()
{
 	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	// Create Skeleton Component to attach a SkeletonMesh to this unit
	Model3DComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Model3DComponent->SetupAttachment(RootComponent);
}

void AUnitModel::SetMesh(USkeletalMesh* Model3D)
{
	check(Model3D);
	Model3DComponent->SetSkeletalMesh(Model3D);
}
