// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxuelWorld.generated.h"

UCLASS()
class VOXUELS_API AVoxuelWorld final : public AActor {
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AVoxuelWorld();

	UPROPERTY(EditAnywhere, Category="World")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category="World")
	TSubclassOf<AActor> Chunk;
	
	UPROPERTY(EditAnywhere, Category="World")
	int DrawDistance = 5;

	UPROPERTY(EditAnywhere, Category="World")
	FIntVector ChunkDimensions = FIntVector(32, 32, 32);

	UPROPERTY(EditAnywhere, Category="World")
	bool Threaded = true;
	
	UPROPERTY(EditAnywhere, Category="Noise")
	float Frequency = 0.03f;

	UPROPERTY(EditAnywhere, Category="Noise")
	int Seed = 1138;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
