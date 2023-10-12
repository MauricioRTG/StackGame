// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockPool.h"

// Sets default values
ABlockPool::ABlockPool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

ABlock* ABlockPool::SpawnBlock()
{
	if (BlockPool.Num() > 0)
	{
		return BlockPool.Pop();
	}
	return nullptr;
}

void ABlockPool::ReturnToPool(ABlock* BlockToReturn)
{
	if (BlockToReturn)
	{
		BlockToReturn->SetActorHiddenInGame(true);
		BlockToReturn->SetInputEnabled(false);
		BlockPool.Add(BlockToReturn);
	}
}

void ABlockPool::InitializePool(int32 PoolSize)
{
	for (int i = 0; i < PoolSize; i++)
	{
		ABlock* NewBlock = GetWorld()->SpawnActor<ABlock>(FVector::ZeroVector, FRotator::ZeroRotator);
		if (NewBlock)
		{
			UStaticMeshComponent* NewBlockMeshComponent = NewBlock->FindComponentByClass<UStaticMeshComponent>();
			if (NewBlockMeshComponent)
			{
				UE_LOG(LogTemp, Log, TEXT("BlockMeshComponent found"));

				NewBlockMeshComponent->SetStaticMesh(BlockMesh);
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("No BlockMeshComponent found"));
			}
			NewBlock->SetActorHiddenInGame(false);
			NewBlock->SetInputEnabled(false);
			BlockPool.Add(NewBlock);
		}
	}
}

// Called when the game starts or when spawned
void ABlockPool::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlockPool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

