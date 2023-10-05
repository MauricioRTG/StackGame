// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StopMoving = false;

	BlockMesh = FindComponentByClass<UStaticMeshComponent>();

	if (!BlockMesh)
	{
		BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block Mesh"));
		RootComponent = BlockMesh;
	}

	InitialLocation = GetActorLocation();
	OscillationFrequency = 1.0f;
	OscillationAmplitud = 100.0f;
	TimeElapsed = 0.0f;

	SplitBlockClass = ABlock::StaticClass();
	PhysicsBlockClass = ABlock::StaticClass();
	ImpulseStrength = 5000.0f;
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
		TimeElapsed += DeltaTime;

		float OscillationValue = OscillationAmplitud * FMath::Sin(OscillationFrequency * TimeElapsed);

		FVector NewLocation = FVector(OscillationValue, 0.0f, 0.0f);

		SetActorLocation(NewLocation);
	}
}

void ABlock::SplitBlock()
{
	FVector SplitLocation = GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);

	//Destroy the current block
	Destroy();

	SpawnPhysicsBlock(SplitLocation, FVector(10.0f, 0.0f, 1.0f));
	SpawnPhysicsBlock(SplitLocation, FVector(-10.0f, 0.0f, 1.0f));
}

void ABlock::SpawnPhysicsBlock(FVector SpawnLocation, FVector ImpulseDirection)
{
	StopMoving = true;
	// Disable collisions for the original block during the split
	BlockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ABlock* NewBlock = GetWorld()->SpawnActor<ABlock>(PhysicsBlockClass, SpawnLocation, FRotator::ZeroRotator);

	if (NewBlock)
	{
		NewBlock->StopMoving = true;
		// Get the current scale of the actor
		FVector CurrentScale = NewBlock->GetActorScale3D();

		// Adjust the scale in the X-axis by half
		FVector NewScale = FVector(CurrentScale.X * 0.5f, CurrentScale.Y, CurrentScale.Z);

		// Set the new scale for the actor
		NewBlock->SetActorScale3D(NewScale);


		if (BlockMesh && NewBlock->BlockMesh)
		{
			NewBlock->BlockMesh->SetStaticMesh(BlockMesh->GetStaticMesh());
		}

		// Disable collisions for the new block during the split
		NewBlock->BlockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (NewBlock->BlockMesh)
		{
			NewBlock->BlockMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			NewBlock->BlockMesh->SetSimulatePhysics(true);
			NewBlock->BlockMesh->AddImpulse(ImpulseDirection * ImpulseStrength);
		}
	}
}

