// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class STACKGAME_API UHUDMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* PlayButton;

	UFUNCTION()
	void OnPlayButtonClicked();

	void NativeConstruct() override;
};
