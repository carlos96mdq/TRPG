// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/BaseTile.h"

ABaseTile::ABaseTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));
	SetRootComponent(DefaultRootComponent);

	GlowingEffectMovement = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlowingEffectMovement"));
	GlowingEffectMovement->SetupAttachment(RootComponent);
	GlowingEffectMovement->SetVisibility(false);
	
	GlowingEffectCombat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlowingEffectCombat"));
	GlowingEffectCombat->SetupAttachment(RootComponent);
	GlowingEffectCombat->SetVisibility(false);
	
	MouseOverEffect = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MouseOverEffect"));
	MouseOverEffect->SetupAttachment(RootComponent);
	MouseOverEffect->SetVisibility(false);
}

void ABaseTile::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind Mouse over begin event
	OnBeginCursorOver.AddUniqueDynamic(this, &ABaseTile::OnTileMouseOver);
	
	// Bind Mouse over end event
	OnEndCursorOver.AddUniqueDynamic(this, &ABaseTile::OnTileMouseOver);
}

void ABaseTile::OnTileMouseOver(AActor* OverActor)
{
	MouseOverEffect->ToggleVisibility();
}

void ABaseTile::SetGlowingEffect(bool bNewState, int32 EffectIdx)
{
	switch (EffectIdx)
	{
	case 1:
		GlowingEffectMovement->SetVisibility(bNewState);
		break;
	case 2:
		GlowingEffectCombat->SetVisibility(bNewState);
		break;
	default:
		GlowingEffectMovement->SetVisibility(bNewState);
		GlowingEffectCombat->SetVisibility(bNewState);
		break;
	}
}

void ABaseTile::SetMovementCost(int NewCost)
{
	MovementCost = NewCost;
}

bool operator<(ABaseTile const& LeftTile, ABaseTile const& RightTile)
{
	return LeftTile.MovementCost < RightTile.MovementCost;
}

bool operator==(ABaseTile const& LeftTile, ABaseTile const& RightTile)
{
	return LeftTile.MovementCost == RightTile.MovementCost;
}