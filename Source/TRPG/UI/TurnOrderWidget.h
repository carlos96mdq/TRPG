// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TurnOrderWidget.generated.h"

// Forward declarations
class UImage;
class ABaseUnit;

/**
 * 
 */
UCLASS()
class TRPG_API UTurnOrderWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Labels
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* PreviousTurn;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* CurrentTurn;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* NextTurn;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* SecondNextTurn;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* ThirdNextTurn;
};
