// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStackGameCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "Camera/CameraComponent.h"
#include "MyStackGamePlayerController.h"
#include "HUDScore.h"
#include "HUDMainMenu.h"

// Sets default values
AMyStackGameCharacter::AMyStackGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	RootComponent = CameraComponent;
}

// Called when the game starts or when spawned
void AMyStackGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Bind AddBlock to player controller
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->InputComponent->BindAction("AddBlock", IE_Pressed, this, &AMyStackGameCharacter::AddBlockToScene);
	}

	//Spawn in scene or world the BlockPool blueprint and then cast it to the ABlockPool class to access its functions
	BP_BlockPool = GetWorld()->SpawnActor<ABlockPool>(BlockPoolClass, FVector::ZeroVector, FRotator::ZeroRotator);
	if (BP_BlockPool)
	{
		BlockPool = Cast<ABlockPool>(BP_BlockPool);
		if (BlockPool)
		{
			BlockPool->InitializePool(PoolSize);
		}
	}

	//Initialize score
	Score = 0;
	
	//Set Camera rotation
	SetActorRotation(CameraRotation);

	//Add Main menu widget to viewport
	UHUDMainMenu* MainMenuWidget = CreateWidget<UHUDMainMenu>(GetWorld(), HUDMainMenuClass);
	if (MainMenuWidget)
	{
		MainMenuWidget->AddToViewport();
		//Set Game to be paused
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		PlayerController->SetInputMode(FInputModeGameAndUI());
		PlayerController->bShowMouseCursor = true;
	}
}

// Called every frame
void AMyStackGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyStackGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMyStackGameCharacter::AddBlockToScene()
{
	UE_LOG(LogTemp, Warning, TEXT("Added Block To Scene"));
	ABlock* NewBlock = BlockPool->SpawnBlock();

	if (NewBlock)
	{
		NewBlock->SetActorHiddenInGame(false);
		NewBlock->SetInputEnabled(true); //Bind input binding to SplitBlockFunction
		NewBlock->SetStopMoving(false);
		NewBlock->SetZOffSet(BlockZOffset);
		BlockZOffset += BlockZOffsetIncrement;
		UpdateCamaraLocation(NewBlock);
		UpdateScore(1);
	}
}

void AMyStackGameCharacter::UpdateScore(int32 Value)
{
	Score += Value;
}

void AMyStackGameCharacter::CreateScoreWidget()
{
	//Set Socore Widget
	UHUDScore* ScoreWidget = CreateWidget<UHUDScore>(GetWorld(), HUDScoreClass);
	//Add widget to viewport and update score text to 0
	if (ScoreWidget)
	{
		ScoreWidget->AddToViewport();
	}
}

void AMyStackGameCharacter::UpdateCamaraLocation(ABlock* NewBlock)
{
	CameraZOffset += CameraZOffsetIncrement;
	FVector NewCamaraLocation = FVector(GetActorLocation().X, GetActorLocation().Y, CameraZOffset);
	SetActorLocation(NewCamaraLocation);
}

