// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitDataIcon.generated.h"

// Forward declarations
class UImage;
class UTextBlock;

struct FUnitDataParams
{
public:
	UTexture2D* Texture2D;
	int32 Armor;
	int32 Life;
	int32 Energy;

	FUnitDataParams(UTexture2D* NewTexture2D, int32 NewArmor, int32 NewLife, int32 NewEnergy)
		: Texture2D(NewTexture2D), Armor(NewArmor), Life(NewLife), Energy(NewEnergy) {}
};

/**
 * 
 */
UCLASS()
class TRPG_API UUnitDataIcon : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta = (BindWidget))
	UImage* UnitIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UnitArmor;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UnitLife;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UnitEnergy;

public:
	void SetData(const FUnitDataParams& UnitDataParams);
};