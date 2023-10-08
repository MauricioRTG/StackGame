// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Set Static Mesh
	BlockMesh = FindComponentByClass<UStaticMeshComponent>();

	if (!BlockMesh)
	{
		BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block Mesh"));
		RootComponent = BlockMesh;
	}

	//Movement
	StopMoving = false;
	InitialLocation = GetActorLocation();
	OscillationFrequency = 1.0f;
	OscillationAmplitud = 100.0f;
	TimeElapsed = 0.0f;
	
	//Spawning
	ImpulseStrength = 5000.0f;
	SizeToResize = 0.5f;
}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();

	// Bind the SplitBlock function to the space key press
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->InputComponent->BindAction("SplitBlock", IE_Pressed, this, &ABlock::SplitBlock);
	}
	
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!StopMoving)
	{
		BlockOscillation(DeltaTime);
	}
}

void ABlock::BlockOscillation(float DeltaTime)
{
	TimeElapsed += DeltaTime;

	float OscillationValue = OscillationAmplitud * FMath::Sin(OscillationFrequency * TimeElapsed);

	FVector NewLocation = FVector(OscillationValue, 0.0f, 0.0f);

	SetActorLocation(NewLocation);
}

void ABlock::SplitBlock()
{
	FVector SplitLocation = GetActorLocation() + FVector(0.0f, 0.0f, 0.0f);

	//Destroy the current block
	Destroy();

	//Spawn two blocks half the size of the original block
	SpawnPhysicsBlock(SplitLocation, FVector(ImpulseDirection.X, ImpulseDirection.Y, ImpulseDirection.Z));
	SpawnPhysicsBlock(SplitLocation, FVector(-ImpulseDirection.X, ImpulseDirection.Y, ImpulseDirection.Z));
}

void ABlock::SpawnPhysicsBlock(FVector SpawnLocation, FVector FunctionImpulseDirection)
{
	StopMoving = true;

	//Spawn block
	ABlock* NewBlock = GetWorld()->SpawnActor<ABlock>(PhysicsBlockClass, SpawnLocation, FRotator::ZeroRotator);

	if (NewBlock)
	{
		NewBlock->StopMoving = true;
		
		ResizeBlock(NewBlock);

		//Add impulse
		if (NewBlock->BlockMesh && BlockMesh)
		{
			//Set same static mesh to new block
			NewBlock->BlockMesh->SetStaticMesh(BlockMesh->GetStaticMesh());
			//Set block to simulate physics so we can add impulse to it
			NewBlock->BlockMesh->SetSimulatePhysics(true);
			NewBlock->BlockMesh->AddImpulse(FunctionImpulseDirection * ImpulseStrength);
		}
	}
}

void ABlock::ResizeBlock(ABlock* NewBlock)
{
	// Get the current scale of the actor
	FVector CurrentScale = NewBlock->GetActorScale3D();

	// Adjust the scale in the X-axis by half
	FVector NewScale = FVector(CurrentScale.X * SizeToResize, CurrentScale.Y, CurrentScale.Z);

	NewBlock->SetActorScale3D(NewScale);
}

