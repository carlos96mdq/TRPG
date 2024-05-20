// Fill out your copyright notice in the Description page of Project Settings.


#include "IndexButton.h"

//void UUIndexedButton::NativeConstruct()
//{
//	UE_LOG(LogTemp, Warning, TEXT("UUIndexedButton::NativeConstruct"));
//	Super::NativeConstruct();
//
//	OnClicked.AddDynamic(this, &UUIndexedButton::ButtonClicked);
//}

UIndexButton::UIndexButton()
{
	UE_LOG(LogTemp, Warning, TEXT("UIndexButton::UIndexButton"));
	OnClicked.AddDynamic(this, &UIndexButton::ButtonClicked);
}

//UIndexButton::UIndexButton(int32 NewIndex)
//:Index(NewIndex)
//{
//	UE_LOG(LogTemp, Warning, TEXT("UIndexButton::UIndexButton inicializo boton %d"), Index);
//	OnClicked.AddDynamic(this, &UIndexButton::ButtonClicked);
//}

void UIndexButton::ButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("ButtonClicked index: %d"), Index);
	OnIndexButtonClicked.Broadcast(Index);
}

void UIndexButton::SetIndex(int32 NewIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("SetIndex %d"), NewIndex);
	Index = NewIndex;
}