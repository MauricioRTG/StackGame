// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlockPool.h"
#include "MyStackGameCharacter.generated.h"

UCLASS()
class STACKGAME_API AMyStackGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyStackGameCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void AddBlockToScene();

	UFUNCTION()
	ABlockPool* GetBlockPool() const { return BlockPool; }

	UFUNCTION()
	int32 GetScore() { return Score; }

	UFUNCTION()
	void UpdateScore(int32 NewScore);

	UFUNCTION()
	void CreateScoreWidget();

	UFUNCTION()
	void CreateEndGameWidget();

private:
	UPROPERTY()
	ABlockPool* BlockPool;

	UPROPERTY()
	ABlockPool* BP_BlockPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BlockPool", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABlockPool> BlockPoolClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlockPool", meta = (AllowPrivateAccess = "true"))
	int32 PoolSize = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BlockPool", meta = (AllowPrivateAccess = "true"))
	float BlockZOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BlockPool", meta = (AllowPrivateAccess = "true"))
	float BlockZOffsetIncrement = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraZOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float CameraZOffsetIncrement = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess))
	TSubclassOf<class UUserWidget> HUDScoreClass;

	UFUNCTION()
	void UpdateCamaraLocation(ABlock* NewBlock);

	UPROPERTY()
	int32 Score;

	/* Main Menu Widget*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDMainMenuClass;

	UPROPERTY()
	APlayerController* PlayerController;

	/* End Game Widget*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> HUDEndGameClass;
};
