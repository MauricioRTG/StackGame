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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float OscillationFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float OscillationAmplitud;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float TimeElapsed; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool StopMoving;

public: 
	UFUNCTION()
		void SplitBlock();

	UFUNCTION()
		void SpawnPhysicsBlock(FVector SpawnLocation, FVector ImpulseDirection);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		TSubclassOf<ABlock> SplitBlockClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
		//EditDefaulysOnly: changes made during runtime or on instances of the object won't affect the default value
		TSubclassOf<ABlock> PhysicsBlockClass;

	//Impulse Strength when spliting
	UPROPERTY(EditAnywhere, Category = "Spawning")
		float ImpulseStrength;
};
