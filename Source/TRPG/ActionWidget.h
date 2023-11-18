// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ActionWidget.generated.h"

// Forward declarations
class UButton;

/**
 * 
 */
UCLASS()
class TRPG_API UActionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void NativeConstruct() override;

	UFUNCTION()
	void CombatPressed();
	
	UFUNCTION()
	void MovePressed();
	
	UFUNCTION()
	void WaitPressed();

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonCombat;	

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonMove;	

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UButton* ButtonWait;	
};
