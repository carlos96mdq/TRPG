// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActiveUnitWidget.generated.h"

// Forward declarations
class UButton;
class UTextBlock;
class ABaseUnit;
enum class EUnitState : uint8;

/**
 * 
 */
UCLASS()
class TRPG_API UActiveUnitWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void NativeConstruct() override;

	UFUNCTION()
	void MovePressed();
	UFUNCTION()
	void Combat0Pressed();
	UFUNCTION()
	void Combat1Pressed();
	UFUNCTION()
	void Combat2Pressed();
	UFUNCTION()
	void Combat3Pressed();
	UFUNCTION()
	void Combat4Pressed();
	UFUNCTION()
	void WaitPressed();

	// Binded to ActiveUnit setting function in GameState to refresh its data in this widget
	void SetUnitData(ABaseUnit* ActiveUnit);
	
	// Binded to ActiveUnit state changed function in PlayerState to refresh buttons in widget
	void ChangeButtonsState(EUnitState NewState);

public:
	// Buttons
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonMove;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonCombat0;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonCombat1;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonCombat2;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonCombat3;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonCombat4;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonWait;

	// Labels
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* LabelName;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* LabelArchetype;
};
