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
	ABlock* BP_Block = GetWorld()->SpawnActor<ABlock>(BlockClass, FVector::ZeroVector, FRotator::ZeroRotator);

	if (BP_Block)
	{
		ABlock* NewBlock = Cast<ABlock>(BP_Block);
		//Resize Block to the same size as previous splited block
		UStaticMeshComponent* NewBlockMeshComponent = NewBlock->GetBlockMeshComponent();
		float CurrentXScale = TopBlock()->GetActorScale3D().X;
		NewBlock->ResizeBlock(NewBlock, CurrentXScale);
		/*float TopBlockBoxSize = BlockPool.Top()->GetBlockMeshComponent()->Bounds.BoxExtent.X;
		float NewSize = TopBlockBoxSize / NewBlockMeshComponent->Bounds.BoxExtent.X;
		NewBlock->ResizeBlock(NewBlock, NewSize);*/
		//NewBlock->GetBlockMeshComponent()->Bounds.BoxExtent.X = TopBlockBoxSize;

		return NewBlock;
	}
	return nullptr;
}

ABlock* ABlockPool::TopBlock()
{
	if (BlockPool.Num() > 0)
	{
		return BlockPool.Top(); //maybe Pop when there is a certain amount of blocks in scene
	}
	return nullptr;
}

void ABlockPool::ReturnToPool(ABlock* BlockToReturn, bool Hide, bool AddToPool)
{
	if (BlockToReturn)
	{
		if (Hide)
		{
			BlockToReturn->SetActorHiddenInGame(true);
			BlockToReturn->SetInputEnabled(false);
			BlockToReturn->SetStopMoving(true);
		}
		if (AddToPool)
		{
			BlockPool.Add(BlockToReturn);
		}
	}
}

void ABlockPool::InitializePool(int32 PoolSize)
{
	for (int i = 0; i < PoolSize; i++)
	{
		if (BlockClass)
		{
			ABlock* BP_Block = GetWorld()->SpawnActor<ABlock>(BlockClass, FVector::ZeroVector, FRotator::ZeroRotator);
			if (BP_Block)
			{
				ABlock* NewBlock = Cast<ABlock>(BP_Block);
				NewBlock->SetActorHiddenInGame(true);
				NewBlock->SetInputEnabled(false);
				NewBlock->SetStopMoving(true);
				BlockPool.Add(NewBlock);
			}	
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

