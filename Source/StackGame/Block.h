// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class STACKGAME_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* BlockMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		FVector InitialLocation;

	//Higher Frequency, means the blocks moves rapidly
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float OscillationFrequency;

	//Higher Amplitud, there is more swing in the blocks movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float OscillationAmplitud;

	//Updates sin function using delta time
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float TimeElapsed; 

	//When the player presses play the block stops moving 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool StopMoving;

	//Spawning blocks
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ABlock> SplitBlockClass = ABlock::StaticClass();

	//Class that is going to be spawned with physics enabled
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
		//EditDefaulysOnly: changes made during runtime or on instances of the object won't affect the default value.
		TSubclassOf<ABlock> PhysicsBlockClass = ABlock::StaticClass();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
		FVector ImpulseDirection = FVector(1.0f, 0.0f, 1.0f);

public: 
	UFUNCTION()
		void SplitBlock();

	UFUNCTION()
		void SpawnPhysicsBlock(FVector SpawnLocation, FVector FunctionImpulseDirection);

	UFUNCTION()
		void ResizeBlock(ABlock* NewBlock);

	UFUNCTION()
		void BlockOscillation(float DeltaTime);

	//Impulse Strength when spliting
	UPROPERTY(EditAnywhere, Category = "Spawning")
		float ImpulseStrength;

	//Size to which the blocks will spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		float SizeToResize;
};
