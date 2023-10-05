// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyStackGamePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class STACKGAME_API AMyStackGamePlayerController : public APlayerController
{
	GENERATED_BODY()

public: 
	AMyStackGamePlayerController();

protected: 
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void OnSpaceKeyPressed();
	
};
