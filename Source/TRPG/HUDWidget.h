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
class UVerticalBox;
class UOverlay;
class ABaseUnit;

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

	UFUNCTION(BlueprintCallable)
	void RestartPressed();

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UActiveUnitWidget* ActiveUnitWidget;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UVerticalBox* PlayerUnits;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UOverlay* GameOverScreen;

	//UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	//UImage* LoadScreen;

	//UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	//UButton* PlayButton;

	void SetPlayerTurn(bool bIsPlayerTurn);
	void UpdateActiveUnitData(ABaseUnit* Unit);
	void GameOver(bool bPlayerWon);
};
