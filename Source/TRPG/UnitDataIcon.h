// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitDataIcon.generated.h"

// Forward declarations
class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class TRPG_API UUnitDataIcon : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* UnitIcon;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* UnitArmor;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* UnitLife;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* UnitEnergy;
};