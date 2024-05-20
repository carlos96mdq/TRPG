// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MMHUDWidget.generated.h"

// Forward declarations
class UMMUnitsSelectionWidget;
class UButton;
class UImage;

/**
 * 
 */
UCLASS()
class TRPG_API UMMHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void NativeConstruct() override;

	UFUNCTION()
	void PlayPressed();

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UMMUnitsSelectionWidget* UnitsSelectionWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* LoadScreen;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* PlayButton;
};
