// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDEndGame.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UHUDEndGame::NativeConstruct()
{
	Super::NativeConstruct();

	ReplayButton->OnClicked.AddUniqueDynamic(this, &UHUDEndGame::OnReplayButtonClicked);
}

void UHUDEndGame::OnReplayButtonClicked()
{
	//Get the name of the current level
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());

	//Restart level
	UGameplayStatics::OpenLevel(GetWorld(), FName(*CurrentLevelName));
}

