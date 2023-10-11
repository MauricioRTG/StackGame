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

	MinBoundsX = -BlockMesh->Bounds.BoxExtent.X;
	MaxBoundsX = BlockMesh->Bounds.BoxExtent.X;
}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();

	HandleInput();
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!StopMoving)
	{
		BlockOscillation(DeltaTime);
	}
	else
	{
		SetInputEnabled(false);

	}
}

void ABlock::HandleInput()
{

	// Bind the SplitBlock function to the space key press
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		if (InputEnabled)
		{
			PlayerController->InputComponent->BindAction("SplitBlock", IE_Pressed, this, &ABlock::SplitBlock);
		}
		else
		{
			PlayerController->InputComponent->ClearBindingsForObject(this);
		}
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
	FVector BlockLocation = GetActorLocation();
	float BoxSizeBound = BlockMesh->Bounds.BoxExtent.X * 2;
	//If it goes to the left we choose the negative bound -100, else 100
	UE_LOG(LogTemp, Log, TEXT("CurrentLocation: %lf"), BlockLocation.X);

	//Split Block if it overlaps with the bottom block, if not let it fall
	if (BlockLocation.X < BoxSizeBound && BlockLocation.X > -BoxSizeBound)
	{
		//If it goes to the right then the bound is 100, if its to the left the bound is -100
		BoxSizeBound = (BlockLocation.X > 0 ? BoxSizeBound : -BoxSizeBound);
		UE_LOG(LogTemp, Log, TEXT("Box overlaps"));

		//Spawn block that is the size of the overlapping region
		float OverlappingWidth = FMath::Abs(BlockLocation.X - BoxSizeBound);
		FVector newLocation = FVector((BlockLocation.X / 2), BlockLocation.Y, BlockLocation.Z);
		float newSize = OverlappingWidth / BoxSizeBound;
		SpawnPhysicsBlock(newLocation, FVector(0.0f, 0.0f, 0.0f), false, newSize);

		//Avoid spawning a second block when the original block is completly overlapping the bottom block
		if (BlockLocation.X != 0)
		{
			//Spawn block that is the size of the non overlapping region
			float NonOverlappingWidth = (BlockMesh->Bounds.BoxExtent.X * 2) - OverlappingWidth;
			FVector NonOverlappingBlockLocation = FVector(BlockLocation.X, BlockLocation.Y, BlockLocation.Z);
			float NonOverlappingBlockSize = NonOverlappingWidth / BoxSizeBound;
			SpawnPhysicsBlock(NonOverlappingBlockLocation, FVector(0.0f, 0.0f, 0.0f), true, NonOverlappingBlockSize);
		}
		
		//Destroy original block
		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Box doesn't overlap"));
		BlockMesh->SetSimulatePhysics(true);
		StopMoving = true;
	}
	
}

void ABlock::SpawnPhysicsBlock(FVector SpawnLocation, FVector FunctionImpulseDirection, bool SimulatePhysics, float NewWidth)
{
	StopMoving = true;

	//Spawn block
	ABlock* NewBlock = GetWorld()->SpawnActor<ABlock>(PhysicsBlockClass, SpawnLocation, FRotator::ZeroRotator);

	if (NewBlock)
	{
		NewBlock->StopMoving = true;
		
		//Set mesh and physics
		if (NewBlock->BlockMesh && BlockMesh)
		{
			//Set same static mesh to new block
			NewBlock->BlockMesh->SetStaticMesh(BlockMesh->GetStaticMesh());
			if (SimulatePhysics)
			{
				//Set block to simulate physics so we can add impulse to it
				NewBlock->BlockMesh->SetSimulatePhysics(true);
				NewBlock->BlockMesh->AddImpulse(FunctionImpulseDirection * ImpulseStrength);
			}
		}

		ResizeBlock(NewBlock, NewWidth);
	}
}

void ABlock::ResizeBlock(ABlock* NewBlock, float NewWidth)
{
	// Get the current scale of the actor
	FVector CurrentScale = NewBlock->GetActorScale3D();

	UE_LOG(LogTemp, Warning, TEXT("CurrentScale: %lf"), CurrentScale.X);

	// Adjust the scale in the X-axis by half
	FVector NewScale = FVector(NewWidth, CurrentScale.Y, CurrentScale.Z);

	UE_LOG(LogTemp, Warning, TEXT("NewScale: %lf"), NewScale.X);

	NewBlock->SetActorScale3D(NewScale);
}

void ABlock::SetInputEnabled(bool Enabled)
{
	InputEnabled = Enabled;
}
