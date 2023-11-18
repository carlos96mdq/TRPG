// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerStash.generated.h"

// Enums
UENUM()
enum class ItemType : uint8 {
	None,
	Weapon,
	Armor,
	Consumable
};

UENUM()
enum class ItemSubtype : uint8 {
	None,
	Sword,
	Axe,
	Shield,
	Helmet,
	Breastplate,
	Potion
};

// Struct to define an item
USTRUCT()
struct FItemStruct
{
	GENERATED_USTRUCT_BODY()

public:

	FItemStruct() {}

	FName Name = TEXT("");
	int32 ItemId = 0;
	ItemType Type = ItemType::None;
	ItemType Subtype = ItemType::None;
};

UCLASS()
class TRPG_API APlayerStash : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Array of items that player has (in guild inventory, in unit's inventory, equipped to unit, etc.)
	TArray<FItemStruct*> ItemsArray;

public:	
	// Sets default values for this actor's properties
	APlayerStash();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
