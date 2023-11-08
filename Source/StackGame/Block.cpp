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
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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

	SetActorLocation(NewLocation);
}

void ABlock::SplitBlock()
{
	FVector BlockLocation = GetActorLocation();
	float PreviousBoxSizeBound = BlockMesh->Bounds.BoxExtent.X * 2;
	ABlock* PreviousBlock;
	FVector PreviousBlockLocation = FVector::ZeroVector;

	//Get previous block
	if(BlockPool && BlockPool->TopBlock() != nullptr)
	{
		PreviousBlock = BlockPool->TopBlock();
		PreviousBoxSizeBound = PreviousBlock->BlockMesh->Bounds.BoxExtent.X * 2;
		PreviousBlockLocation = PreviousBlock->GetActorLocation();
	}

	UE_LOG(LogTemp, Log, TEXT("CurrentLocation: %lf"), BlockLocation.X);

	//Block Size
	float CurrentBlockBoxSize = BlockMesh->Bounds.BoxExtent.X * 2;

	//Split Block if it overlaps with the bottom block, if not let it fall
	if (BlockLocation.X < (PreviousBlockLocation.X + CurrentBlockBoxSize) && BlockLocation.X > (PreviousBlockLocation.X - CurrentBlockBoxSize))
	{
		//Calculate Bounds from previous block
		MaxBoundsX = (PreviousBlockLocation.X + CurrentBlockBoxSize);
		MinBoundsX = (PreviousBlockLocation.X - CurrentBlockBoxSize);
		
		
		UE_LOG(LogTemp, Log, TEXT("////Box overlaps"));
		UE_LOG(LogTemp, Log, TEXT("MinBoundsX: %f"), MinBoundsX);
		UE_LOG(LogTemp, Log, TEXT("MaxBoundsX: %f"), MaxBoundsX);

		//If it goes to the right then the bound is 100, if its to the left the bound is -100
		PreviousBoxSizeBound = (BlockLocation.X < PreviousBlockLocation.X ? -PreviousBoxSizeBound : PreviousBoxSizeBound );

		UE_LOG(LogTemp, Log, TEXT("BoxSizeBound: %f"), PreviousBoxSizeBound);
		UE_LOG(LogTemp, Log, TEXT("PreviousBlockXLocation: %f"), PreviousBlockLocation.X);

		
		//Spawn block that is the size of the overlapping region and return overlapping width
		float OverlappingWidth = SpawnOverlappingBlock(BlockLocation, PreviousBlockLocation, PreviousBoxSizeBound);

		//Avoid spawning a second block when the original block is completly overlapping the bottom block
		if (BlockLocation.X != PreviousBlockLocation.X)
		{
			//Spawn block that is the size of the non overlapping region
			SpawnNonOverlappingBlock(BlockLocation, PreviousBlockLocation, PreviousBoxSizeBound, OverlappingWidth);
		}
		//Destroy original block
		Destroy();

		//When finished spliting block, add new block on top of this 
		if (PlayerCharacter)
		{
			PlayerCharacter->UpdateScore(1);
			PlayerCharacter->AddBlockToScene();
		}
	}
	else
	{
		//Let the block fall when is outside boundaries
		UE_LOG(LogTemp, Log, TEXT("Box doesn't overlap"));
		BlockMesh->SetSimulatePhysics(true);
		StopMoving = true;

		//Add the End Game widget to the viewport
		if (PlayerCharacter)
		{
			PlayerCharacter->CreateEndGameWidget();
			//Set input mode to only let the user interact with the UI elements
			if (PlayerController)
			{
				PlayerController->SetInputMode(FInputModeUIOnly());
				PlayerController->bShowMouseCursor = true;
			}
		}
	}
}

float ABlock::SpawnOverlappingBlock(FVector CurrentBlockLocation, FVector PreviousBlockLocation, float PreviousBoxSizeBound)
{
	UE_LOG(LogTemp, Log, TEXT("//Overlapping Region"));
	float InsideBlockEdge = (CurrentBlockLocation.X < PreviousBlockLocation.X ? CurrentBlockLocation.X + BlockMesh->Bounds.BoxExtent.X : CurrentBlockLocation.X - BlockMesh->Bounds.BoxExtent.X);
	UE_LOG(LogTemp, Log, TEXT("InsideBlockEdge: %f"), InsideBlockEdge);
	float OverlappingWidth = FMath::Abs(InsideBlockEdge - (CurrentBlockLocation.X < PreviousBlockLocation.X ? (MinBoundsX / 2) : (MaxBoundsX / 2)));
	UE_LOG(LogTemp, Log, TEXT("OverlappingWidth: %f"), OverlappingWidth);
	FVector newLocation = FVector((CurrentBlockLocation.X < PreviousBlockLocation.X ? InsideBlockEdge - (OverlappingWidth / 2) : InsideBlockEdge + (OverlappingWidth / 2)), CurrentBlockLocation.Y, CurrentBlockLocation.Z);
	UE_LOG(LogTemp, Log, TEXT("NewLocation: %f"), newLocation.X);
	float newSize = OverlappingWidth / PreviousBoxSizeBound;
	UE_LOG(LogTemp, Log, TEXT("NewSize: %f"), newSize);
	SpawnPhysicsBlock(newLocation, FVector(0.0f, 0.0f, 0.0f), false, newSize, true);

	return OverlappingWidth;
}

void ABlock::SpawnNonOverlappingBlock(FVector CurrentBlockLocation, FVector PreviousBlockLocation, float PreviousBoxSizeBound, float OverlappingWidth)
{
	UE_LOG(LogTemp, Log, TEXT("NonOverlapping Region"));
	float OutsideBlockEdge = (CurrentBlockLocation.X < PreviousBlockLocation.X ? CurrentBlockLocation.X - BlockMesh->Bounds.BoxExtent.X : CurrentBlockLocation.X + BlockMesh->Bounds.BoxExtent.X);
	UE_LOG(LogTemp, Log, TEXT("OutsideBlockEdge: %f"), OutsideBlockEdge);
	float NonOverlappingWidth = (BlockMesh->Bounds.BoxExtent.X * 2) - OverlappingWidth;
	UE_LOG(LogTemp, Log, TEXT("NonOverlappingWidth: %f"), NonOverlappingWidth);
	FVector NonOverlappingBlockLocation = FVector((CurrentBlockLocation.X < PreviousBlockLocation.X ? OutsideBlockEdge + (NonOverlappingWidth / 2) : OutsideBlockEdge - (NonOverlappingWidth / 2)), CurrentBlockLocation.Y, CurrentBlockLocation.Z);
	UE_LOG(LogTemp, Log, TEXT("NonOverlappingBlockLocation: %f"), NonOverlappingBlockLocation.X);
	float NonOverlappingBlockSize = NonOverlappingWidth / PreviousBoxSizeBound;
	UE_LOG(LogTemp, Log, TEXT("NonOverlappingBlockSize: %f"), NonOverlappingBlockSize);
	SpawnPhysicsBlock(NonOverlappingBlockLocation, FVector(0.0f, 0.0f, 0.0f), true, NonOverlappingBlockSize, false);
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
