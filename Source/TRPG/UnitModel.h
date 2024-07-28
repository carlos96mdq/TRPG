// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitModel.generated.h"

UCLASS()
class TRPG_API AUnitModel : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	USkeletalMeshComponent* Model3DComponent;

public:	
	// Sets default values for this actor's properties
	AUnitModel();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Set a 3D Model to this unit Model3DComponent
	void SetMesh(USkeletalMesh* Model3D);

};