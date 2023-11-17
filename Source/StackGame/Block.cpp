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
}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();

	MinBoundsX = -BlockMesh->Bounds.BoxExtent.X;
	MaxBoundsX = BlockMesh->Bounds.BoxExtent.X;

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
		//When the block stops moving it shouldn't respond to player input anymore
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
	//Updates oscilaltion value according to the delta time, which the behavior is achieved with the Sin() function
	float OscillationValue = OscillationAmplitud * FMath::Sin(OscillationFrequency * TimeElapsed);

	FVector NewLocation = FVector(OscillationValue, 0.0f, ZOffset);

	SetActorLocation(NewLocation);
}

void ABlock::SplitBlock()
{
	FVector BlockLocation = GetActorLocation();
	double PreviousBoxSizeBound = BlockMesh->Bounds.BoxExtent.X * 2;
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
	double CurrentBlockBoxSize = BlockMesh->Bounds.BoxExtent.X * 2;

	/*Laft and rigth bound based in previous block in pool
	When the previous location is not 0 then we add an offset */
	double PreviousBlockRightBound = (PreviousBlockLocation.X == 0.0f ? (PreviousBoxSizeBound + PreviousBlockLocation.X) : (PreviousBoxSizeBound + PreviousBlockLocation.X) + 0.5);
	double PreviousBlockLeftBound = (PreviousBlockLocation.X == 0.0f ? (PreviousBlockLocation.X - PreviousBoxSizeBound) : (PreviousBlockLocation.X - PreviousBoxSizeBound) - 0.5);

	//Split Block if it overlaps with the bottom block, if not let it fall
	if (BlockLocation.X < PreviousBlockRightBound && BlockLocation.X > PreviousBlockLeftBound)
	{
		
		//If it goes to the right then the bound is 100, if its to the left the bound is -100
		//PreviousBoxSizeBound = (BlockLocation.X < PreviousBlockLocation.X ? -PreviousBoxSizeBound : PreviousBoxSizeBound );

		UE_LOG(LogTemp, Log, TEXT("PreviousBoxSizeBound: %f"), PreviousBoxSizeBound);
		UE_LOG(LogTemp, Log, TEXT("PreviousBlockXLocation: %f"), PreviousBlockLocation.X);

		/*When the moving block is stopped and it has approximately the same location as the previous block, then don't spawn the non overlapping block*/
		double NonOverlappingWidth = 0.0f;
		if (BlockLocation.X > (PreviousBlockLocation.X + 4) || BlockLocation.X < (PreviousBlockLocation.X - 4))
		{
			//Spawn block that is the size of the non overlapping region
			NonOverlappingWidth = SpawnNonOverlappingBlock(BlockLocation, PreviousBlockLocation, PreviousBoxSizeBound, CurrentBlockBoxSize);
		}

		//Spawn block that is the size of the overlapping region and return overlapping width
		SpawnOverlappingBlock(BlockLocation, PreviousBlockLocation, PreviousBoxSizeBound, NonOverlappingWidth);
		
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

void ABlock::SpawnOverlappingBlock(FVector CurrentBlockLocation, FVector PreviousBlockLocation, double PreviousBoxSizeBound, double NonOverlappingWidth)
{
	UE_LOG(LogTemp, Log, TEXT("//Overlapping Region"));

	/*We obtain the OverlappingWith by subtracting the previosu box size (it can be also the current one because it hasn't change sizes yet) with the NonOverlappingWidth*/
	double OverlappingWidth = PreviousBoxSizeBound - NonOverlappingWidth;
	/*Clamp OverlappingWidth to be between 0 and 100 (100 is the size of the original block size)*/
	//OverlappingWidth = FMath::Clamp(OverlappingWidth, 0.0f, 100.0f);
	UE_LOG(LogTemp, Log, TEXT("OverlappingWidth: %f"), OverlappingWidth);

	/*The idea is similar when obtaining the new NonOverlappingBlockLocation, but here we don't need an outside location but rather and inside location so the location of origin or reference will be the previous block location,
	then we just need to sum the (NonOverlappingWith / 2) which can also be (PreviousBoxSizeBound - OverlappingWidth) / 2, so in other words, we obtain teh location by taking into account the previous block location and the size of the block*/
	FVector OverlappingBlockLocation = FVector((CurrentBlockLocation.X < PreviousBlockLocation.X ? (PreviousBlockLocation.X - (NonOverlappingWidth / 2)) : (PreviousBlockLocation.X + (NonOverlappingWidth /2))), CurrentBlockLocation.Y, CurrentBlockLocation.Z);
	UE_LOG(LogTemp, Log, TEXT("OverlappingBlockLocation: %f"), OverlappingBlockLocation.X);

	/*Divided by the previous box size to determine the scale of the block according to the new width,
	when the previous block location is not 0 then we add an offset to the block size,
	which we clamp between a range to avoid having excesive large spawned blocks when the blocks have small sizes and they very close to each other
	Between a small arbitrary value (e.g. 4) to 100, which 100 is the size of the first block in scene*/
	double OverlappingBlockSize = (PreviousBlockLocation.X == 0 ? OverlappingWidth : FMath::Clamp(OverlappingWidth - 11.0f, 0.5f, 100.0f) ) / PreviousBoxSizeBound;
	UE_LOG(LogTemp, Log, TEXT("OverlappingBlockSize: %f"), OverlappingBlockSize);

	SpawnPhysicsBlock(OverlappingBlockLocation, FVector::ZeroVector, false, OverlappingBlockSize, true);
}

double ABlock::SpawnNonOverlappingBlock(FVector CurrentBlockLocation, FVector PreviousBlockLocation, double PreviousBoxSize, double CurrentBlockBoxSize)
{
	UE_LOG(LogTemp, Log, TEXT("//NonOverlapping Region"));

	/*So the idea of this calcuation is to obtain the dimensions of the current block and the previous block, where we substract the current block x dimmension with the previous block X dimension,
	Also taking into consideration the thier respective current locations in the space, because this are going to constantly change*/
	double NonOverlappingWidth = FMath::Abs((CurrentBlockLocation.X + (CurrentBlockBoxSize / 2)) - ((PreviousBoxSize / 2) + PreviousBlockLocation.X));
	/*Clamp NonOverlappingWidthto only be between 0 to 100, which 100 is the size of the first block in scene*/
	//NonOverlappingWidth = FMath::Clamp(NonOverlappingWidth, 0.0f, 100.0f);
	UE_LOG(LogTemp, Log, TEXT("NonOverlappingWidth: %f"), NonOverlappingWidth);

	/*The idea of this calculation is that when the block is in the right side we first sum the previous block location and size and then sum it with the NonOverlappingWidth that is divded by two
	In other words, we obtain first obtain a location that is outside the previous block (Which is half the size of the cube) but is not exactly at the edge, we obtain this edge location by substracting by the NonOverlappingWidth and dividing by 2 this width*/
	FVector NonOverlappingBlockLocation = FVector((CurrentBlockLocation.X < PreviousBlockLocation.X ?  ((PreviousBlockLocation.X - PreviousBoxSize) + (NonOverlappingWidth / 2)) : ((PreviousBlockLocation.X + PreviousBoxSize) - (NonOverlappingWidth / 2))), CurrentBlockLocation.Y, CurrentBlockLocation.Z);
	UE_LOG(LogTemp, Log, TEXT("NonOverlappingBlockXLocation: %f"), NonOverlappingBlockLocation.X);

	/*Divided by the previous box size to determine the scale of the block according to the new width,
	when the previous block location is not 0 then we add an offset to the block size
	which we clamp between a range to avoid having excesive large spawned blocks when the blocks have small sizes and they very close to each other.
	Between a small arbitrary value (e.g. 4) to 100, which 100 is the size of the first block in scene*/
	double NonOverlappingBlockSize = (PreviousBlockLocation.X == 0 ? NonOverlappingWidth : FMath::Clamp(NonOverlappingWidth - 11.0f, 0.5f, 100.0f)) / PreviousBoxSize;
	UE_LOG(LogTemp, Log, TEXT("NonOverlappingBlockSize: %f"), NonOverlappingBlockSize);

	SpawnPhysicsBlock(NonOverlappingBlockLocation, FVector::ZeroVector, true, NonOverlappingBlockSize, false);

	return NonOverlappingWidth;
}

void ABlock::SpawnPhysicsBlock(FVector SpawnLocation, FVector FunctionImpulseDirection, bool SimulatePhysics, double NewWidth, bool AddToPool)
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

		//Resize block 
		ResizeBlock(NewBlock, NewWidth);
		UE_LOG(LogTemp, Log, TEXT("New BoxSizeBound: %f"), NewBlock->BlockMesh->Bounds.BoxExtent.X * 2);

		//Add splited block to the pool
		if (BlockPool && AddToPool)
		{
			BlockPool->ReturnToPool(NewBlock, false, true);
		}
	}
}

void ABlock::ResizeBlock(ABlock* NewBlock, double NewWidth)
{
	// Get the current scale of the actor
	FVector CurrentScale = NewBlock->GetActorScale3D();

	UE_LOG(LogTemp, Warning, TEXT("CurrentScale: %lf"), CurrentScale.X);

	// Adjust the scale in the X-axis by half
	FVector NewScale = FVector(NewWidth, CurrentScale.Y, CurrentScale.Z);

	UE_LOG(LogTemp, Warning, TEXT("NewScale: %lf"), NewScale.X);

	NewBlock->SetActorScale3D(NewScale);
}
