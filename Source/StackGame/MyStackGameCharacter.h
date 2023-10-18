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

private:
	UPROPERTY()
	ABlockPool* BlockPool;

	UPROPERTY()
	ABlockPool* BP_BlockPool;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BlockPool", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABlockPool> BlockPoolClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BlockPool", meta = (AllowPrivateAccess = "true"))
	int32 PoolSize = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BlockPool", meta = (AllowPrivateAccess = "true"))
	float BlockZOffsetIncrement = 0;

};
