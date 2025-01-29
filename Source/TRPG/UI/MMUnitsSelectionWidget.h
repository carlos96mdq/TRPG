// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MMUnitsSelectionWidget.generated.h"

// Forward declarations
class UButton;
class UImage;

/**
 * 
 */
UCLASS()
class TRPG_API UMMUnitsSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void NativeConstruct() override;

	// Array of selectable units images
	TArray<UImage*> ImagesArray;

	// Binded to available units setting function in PlayerState to refresh its data in this widget
	void SetSelectableUnits();

	UFUNCTION()
	void SelectUnit0();
	UFUNCTION()
	void SelectUnit1();
	UFUNCTION()
	void SelectUnit2();

	void SelectUnit(int32 UnitIndex);

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonUnit0;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonUnit1;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonUnit2;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* Image0;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* Image1;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UImage* Image2;
};
