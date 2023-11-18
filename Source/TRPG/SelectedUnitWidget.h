// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectedUnitWidget.generated.h"

// Forward declarations
class UTextBlock;
class ABaseUnit;

/**
 * 
 */
UCLASS()
class TRPG_API USelectedUnitWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void NativeConstruct() override;

	// Binded to SelectedUnit setting function in PlayerState to refresh its data in this widget
	void SetUnitData(ABaseUnit* SelectedUnit);

public:
	// Labels
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* LabelName;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* LabelArchetype;
};
