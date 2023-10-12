// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStackGamePlayerController.h"
#include <Kismet/GameplayStatics.h>

AMyStackGamePlayerController::AMyStackGamePlayerController()
{

}

void AMyStackGamePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Your initialization code here
}

void AMyStackGamePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

	InputComponent->BindAction("SplitBlock", IE_Pressed, this, &AMyStackGamePlayerController::OnSpaceKeyPressed);
}

void AMyStackGamePlayerController::OnSpaceKeyPressed()
{
    // Implement your logic for when the space key is pressed
    UE_LOG(LogTemp, Warning, TEXT("Space key pressed in player controller!"));
    // Add more logic here as needed
}
