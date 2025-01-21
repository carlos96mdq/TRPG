// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "IndexButton.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnIndexedButtonClickedEvent, int32)

/**
 * 
 */
UCLASS()
class TRPG_API UIndexButton : public UButton
{
	GENERATED_BODY()
	
	int32 Index = 0;

protected:
	// Called when the game starts or when spawned
	//virtual void NativeConstruct() override;
	UIndexButton();
	//UIndexButton(int32 NewIndex);

	// Called when the indexed button is clicked
	UFUNCTION()
	void ButtonClicked();

public:
	FOnIndexedButtonClickedEvent OnIndexButtonClicked;

	void SetIndex(int32 NewIndex);
};
