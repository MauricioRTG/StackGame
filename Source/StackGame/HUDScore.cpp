// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDScore.h"
#include "Components/TextBlock.h"
#include "MyStackGameCharacter.h"
#include "Kismet/GameplayStatics.h"

FText UHUDScore::UpdateScoreTextBlock()
{
	AMyStackGameCharacter* PlayerCharacter = Cast<AMyStackGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		return FText::FromString(FString::FromInt(PlayerCharacter->GetScore()));
	}

	return FText::FromString("NULL");
}

bool UHUDScore::Initialize()
{
	bool Success = Super::Initialize();

	if (!Success) { return false; }

	//Bind function to Text widget content
	if (ScoreText)
	{
		ScoreText->TextDelegate.BindUFunction(this, "UpdateScoreTextBlock");
	}

	return true;
}
