// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDEndGame.generated.h"

/**
 * 
 */
UCLASS()
class STACKGAME_API UHUDEndGame : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* ReplayButton;

	UFUNCTION()
	void OnReplayButtonClicked();

	void NativeConstruct() override;
};
