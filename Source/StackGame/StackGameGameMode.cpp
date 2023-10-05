// Copyright Epic Games, Inc. All Rights Reserved.

#include "StackGameGameMode.h"
#include "StackGamePlayerController.h"
#include "StackGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

AStackGameGameMode::AStackGameGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AStackGamePlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}