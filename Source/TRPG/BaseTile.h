// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseTile.generated.h"

// Enums
UENUM(BlueprintType)
enum class TileType : uint8 {
	Normal,
	Wall
};

UCLASS()
class TRPG_API ABaseTile : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when tile is clicked by mouse
	UFUNCTION()
	void OnTileClicked(AActor* ClickedActor, FKey ButtonPressed);
	
	// Called when mouse is over this tile
	UFUNCTION()
	void OnTileMouseOver(AActor* OverActor);

	// Default Plane component
	UPROPERTY(EditDefaultsOnly)
	USceneComponent* DefaultRootComponent;

	// Glowing component
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* GlowingEffect;
	
	// Glowing component
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MouseOverEffect;

	// Tile type
	UPROPERTY(EditDefaultsOnly)
	TileType Type;

	// Tile movement cost from the center unit that wants to move
	int MovementCost = -1;

public:	
	// Sets default values for this actor's properties
	ABaseTile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Set the glowing effect on/off
	void SetGlowingEffect(bool bNewState=true);

	// Set the movement cost at the moment
	void SetMovementCost(int NewCost);

	// Operator overload <
	friend bool operator<(ABaseTile const& LeftTile, ABaseTile const& RightTile);
	
	// Operator overload =
	friend bool operator==(ABaseTile const& LeftTile, ABaseTile const& RightTile);

	TileType GetType() const { return Type; };
	int GetMovementCost() const { return MovementCost; };
};
