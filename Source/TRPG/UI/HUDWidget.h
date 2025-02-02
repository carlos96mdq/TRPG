// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

// Forward declarations
class UActiveUnitWidget;
class USelectedUnitWidget;
class UButton;
class UImage;
class UVerticalBox;
class UOverlay;
class ABaseUnit;
class UUnitDataIcon;

/**
 * 
 */
UCLASS()
class TRPG_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

	TMap<int32, UUnitDataIcon*> UnitDataIconList;
	TMap<int32, UUnitDataIcon*> EnemyDataIconList;

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
	UVerticalBox* EnemyUnits;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UOverlay* GameOverScreen;

	//UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	//UImage* LoadScreen;

	void SetPlayerTurn(bool bIsPlayerTurn);
	void CreateUnitDataWidget(ABaseUnit* Unit, const TSubclassOf<UUnitDataIcon>& UnitDataIconClass, bool IsThisPlayer);
	void UpdateUnitData(ABaseUnit* Unit);
	void UpdateActiveUnitData(ABaseUnit* Unit);
	void GameOver(bool bPlayerWon);
};
