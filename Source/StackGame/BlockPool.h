// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.h"
#include "BlockPool.generated.h"

UCLASS()
class STACKGAME_API ABlockPool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlockPool();

	ABlock* SpawnBlock();

	void ReturnToPool(ABlock* BlockToReturn);

	void Initialize(int32 PoolSize);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	TArray<ABlock*> BlockPool;
};
