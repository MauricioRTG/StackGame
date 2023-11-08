// Fill out your copyright notice in the Description page of Project Settings.


#include "HUDMainMenu.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "MyStackGameCharacter.h"

void UHUDMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddUniqueDynamic(this, &UHUDMainMenu::OnPlayButtonClicked);
}

void UHUDMainMenu::OnPlayButtonClicked()
{
	//Set game to a paused state
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	APlayerController* PlayerController =  UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	//Interact with game elements only and not show cursor in screeen
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;

	//Player character reference
	AMyStackGameCharacter* PlayerCharacter = Cast<AMyStackGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (PlayerCharacter)
	{
		//Add Score Widget to viewport
		PlayerCharacter->CreateScoreWidget();
		//Add block to scene
		PlayerCharacter->AddBlockToScene();
	}



	//Remove widget from viewport
	this->RemoveFromParent();
}

