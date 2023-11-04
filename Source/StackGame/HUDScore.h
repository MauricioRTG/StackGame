// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDScore.generated.h"

/**
 * 
 */
UCLASS()
class STACKGAME_API UHUDScore : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ScoreText;

	UFUNCTION(BlueprintCallable, Category = "UI")
	FText UpdateScoreTextBlock();

	bool Initialize() override;
};

