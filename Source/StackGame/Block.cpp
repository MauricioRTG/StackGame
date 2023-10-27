// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"
#include "BlockPool.h"
#include <Kismet/GameplayStatics.h>
#include "MyStackGameCharacter.h"

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

	SplitBlockClass = ABlock::StaticClass();
	PhysicsBlockClass = ABlock::StaticClass();

	PlayerCharacter = Cast<AMyStackGameCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Log, TEXT("MyStackGameCharacter not found"));
	}
	
	BlockPool = PlayerCharacter->GetBlockPool();
	if (!BlockPool)
	{
		UE_LOG(LogTemp, Log, TEXT("BlockPool not found"));
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
	else
	{
		SetInputEnabled(false);
	}
}

void ABlock::HandleInput()
{
	if (InputEnabled)
	{
		// Bind the SplitBlock function to the space key press
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			PlayerController->InputComponent->BindAction("SplitBlock", IE_Pressed, this, &ABlock::SplitBlock);
		}
	}
}

void ABlock::SetZOffSet(float NewZlocation)
{
	ZOffset = NewZlocation;
	UE_LOG(LogTemp, Log, TEXT("ZOffset Changed to: %f"), ZOffset);
}

void ABlock::SetStopMoving(bool FunctionStopMoving)
{
	StopMoving = FunctionStopMoving;
}

void ABlock::SetBlockMesh(UStaticMesh* FunctionBlockMesh)
{
	BlockMesh->SetStaticMesh(FunctionBlockMesh);

}

UStaticMeshComponent* ABlock::GetBlockMeshComponent()
{
	return BlockMesh;
}

void ABlock::SetInputEnabled(bool Enabled)
{
	InputEnabled = Enabled;
	HandleInput();
}

void ABlock::BlockOscillation(float DeltaTime)
{
	TimeElapsed += DeltaTime;

	float OscillationValue = OscillationAmplitud * FMath::Sin(OscillationFrequency * TimeElapsed);

	FVector NewLocation = FVector(OscillationValue, 0.0f, ZOffset);
	//UE_LOG(LogTemp, Log, TEXT("ZOffset in Oscillation: %f"), ZOffset);

	SetActorLocation(NewLocation);
}

void ABlock::SplitBlock()
{
	FVector BlockLocation = GetActorLocation();
	float BoxSizeBound = BlockMesh->Bounds.BoxExtent.X * 2;
	ABlock* PreviousBlock;
	float PreviousBlockXLocation = 0;

	if(BlockPool && BlockPool->TopBlock() != nullptr)
	{
		PreviousBlock = BlockPool->TopBlock();
		BoxSizeBound = PreviousBlock->BlockMesh->Bounds.BoxExtent.X * 2;
		PreviousBlockXLocation = PreviousBlock->GetActorLocation().X;
	}

	UE_LOG(LogTemp, Log, TEXT("CurrentLocation: %lf"), BlockLocation.X);
	//UE_LOG(LogTemp, Log, TEXT("BoxSizeBound: %f"), BoxSizeBound);

	//Calculate box bound offset according to resize orignial size is 1 
	FVector CurrentScale = GetActorScale3D();
	float BoxBoundsOffset = 10;
	float CurrentBlockBoxSize = BlockMesh->Bounds.BoxExtent.X * 2;

	//Split Block if it overlaps with the bottom block, if not let it fall
	if (BlockLocation.X < (PreviousBlockXLocation + CurrentBlockBoxSize) && BlockLocation.X > (PreviousBlockXLocation - CurrentBlockBoxSize))
	{
		//If it goes to the right then the bound is 100, if its to the left the bound is -100
		//BoxSizeBound = (BlockLocation.X > (BoxSizeBound + BoxBoundsOffset) / 2 ? (BoxSizeBound + BoxBoundsOffset) : (-BoxSizeBound + BoxBoundsOffset));
		MaxBoundsX = (PreviousBlockXLocation + CurrentBlockBoxSize);
		MinBoundsX = (PreviousBlockXLocation - CurrentBlockBoxSize);
		UE_LOG(LogTemp, Log, TEXT("Box overlaps"));

		//Spawn block that is the size of the overlapping region
		float OverlappingWidth = FMath::Abs(GetActorLocation().X - (GetActorLocation().X < PreviousBlockXLocation ? MinBoundsX : MaxBoundsX));
		FVector newLocation = FVector((GetActorLocation().X < PreviousBlockXLocation ? (MinBoundsX + OverlappingWidth) / 2.0f : (MaxBoundsX - OverlappingWidth) / 2.0f), BlockLocation.Y, BlockLocation.Z);
		float newSize = OverlappingWidth / BoxSizeBound;
		SpawnPhysicsBlock(newLocation, FVector(0.0f, 0.0f, 0.0f), false, newSize, true);

		//Avoid spawning a second block when the original block is completly overlapping the bottom block
		if (BlockLocation.X != PreviousBlockXLocation)
		{
			//Spawn block that is the size of the non overlapping region
			float NonOverlappingWidth = (BlockMesh->Bounds.BoxExtent.X * 2) - OverlappingWidth;
			FVector NonOverlappingBlockLocation = FVector((GetActorLocation().X < PreviousBlockXLocation ? (MinBoundsX + NonOverlappingWidth) / 2.0f : (MaxBoundsX - NonOverlappingWidth) / 2.0f), BlockLocation.Y, BlockLocation.Z);
			float NonOverlappingBlockSize = NonOverlappingWidth / BoxSizeBound;
			SpawnPhysicsBlock(NonOverlappingBlockLocation, FVector(0.0f, 0.0f, 0.0f), true, NonOverlappingBlockSize, false);
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

	//When finished spliting block, add new block on top of this 
	if (PlayerCharacter)
	{
		PlayerCharacter->AddBlockToScene();
	}
	
}

void ABlock::SpawnPhysicsBlock(FVector SpawnLocation, FVector FunctionImpulseDirection, bool SimulatePhysics, float NewWidth, bool AddToPool)
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
		UE_LOG(LogTemp, Log, TEXT("New BoxSizeBound: %f"), NewBlock->BlockMesh->Bounds.BoxExtent.X * 2);

		if (BlockPool && AddToPool)
		{
			BlockPool->ReturnToPool(NewBlock, false, true);
		}
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
