// Fill out your copyright notice in the Description page of Project Settings.


#include "MyStackGameCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "MyStackGamePlayerController.h"

// Sets default values
AMyStackGameCharacter::AMyStackGameCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyStackGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	BlockPool = GetWorld()->SpawnActor<ABlockPool>(FVector::ZeroVector, FRotator::ZeroRotator);
	BlockPool->InitializePool(1);
	
	//To avoid handling in player controller, because is not a complex logic
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerController->InputComponent->BindAction("SplitBlock", IE_Released, this, &AMyStackGameCharacter::AddBlockToScene);
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
		NewBlock->SetInputEnabled(true);
		NewBlock->SetYOffset(200.0f);
	}
}

