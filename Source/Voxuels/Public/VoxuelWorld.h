// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk/VoxuelChunkBase.h"
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

	UPROPERTY(EditAnywhere, Category="Noise")
	float Frequency = 0.03f;

	UPROPERTY(EditAnywhere, Category="Noise")
	int Seed = 1138;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool Threaded = true;
	
	UPROPERTY(EditAnywhere, Category="Debug")
	bool IterativeRendering = false;

	UPROPERTY(EditAnywhere, Category="Debug")
	FIntVector RenderToChunk = FIntVector::ZeroValue;
	
	UPROPERTY(EditAnywhere, Category="Debug")
	FIntVector RenderToBlock = FIntVector::ZeroValue;

protected:
	UFUNCTION(CallInEditor, Category = "Debug")
	void RenderAll();

	UFUNCTION(CallInEditor, Category = "Debug")
	void RenderChunk();

	UFUNCTION(CallInEditor, Category = "Debug")
	void Clear();

private:
	bool RenderEnvironmentInitialized = false;
	TArray<TObjectPtr<AVoxuelChunkBase>> Chunks;

	void InitializeRenderEnvironment();
	
	void DoRenderAll();
	void DoRenderIteration();
};
