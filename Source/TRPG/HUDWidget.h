// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

// Forward declarations
class UActiveUnitWidget;
class USelectedUnitWidget;
class UTurnOrderWidget;
class UButton;
class UImage;

/**
 * 
 */
UCLASS()
class TRPG_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void NativeConstruct() override;

	//UFUNCTION()
	//void PlayPressed();

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UActiveUnitWidget* ActiveUnitWidget;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	USelectedUnitWidget* SelectedUnitWidget;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTurnOrderWidget* TurnOrderWidget;
	
	//UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	//UImage* LoadScreen;

	//UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	//UButton* PlayButton;
};
