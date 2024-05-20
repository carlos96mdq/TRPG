// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitModel.h"

// Sets default values
AUnitModel::AUnitModel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Skeleton Component to attach a SkeletonMesh to this unit
	Model3DComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Body"));
	Model3DComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AUnitModel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnitModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUnitModel::SetMesh(USkeletalMesh* Model3D)
{
	check(Model3D);
	Model3DComponent->SetSkeletalMesh(Model3D);
}
