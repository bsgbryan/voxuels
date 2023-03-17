// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk/VoxuelChunkBase.h"
#include "GameFramework/Actor.h"
#include "VoxuelWorld.generated.h"

class UVoxuelDecoratorBevelBase;

UCLASS()
class VOXUELS_API AVoxuelWorld final : public AActor {
	GENERATED_BODY()
	
public:
	AVoxuelWorld();

	UPROPERTY(EditAnywhere, Category="World")
	FVector Offset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category="World")
	TSubclassOf<AActor> Chunk;
	
	UPROPERTY(EditAnywhere, Category="World")
	FIntVector DrawDistance = FIntVector(5, 5, 1);

	UPROPERTY(EditAnywhere, Category="World")
	FIntVector ChunkDimensions = FIntVector(32, 32, 32);

	UPROPERTY(EditAnywhere, Category="World")
	bool Threaded = true;
	
	UPROPERTY(EditAnywhere, Category="World")
	bool IterativeRendering = false;

	UPROPERTY(EditAnywhere, Category="World")
	FIntVector RenderFromChunk = FIntVector::ZeroValue;

	UPROPERTY(EditAnywhere, Category="World")
	FIntVector RenderToChunk = FIntVector::ZeroValue;
	
	UPROPERTY(EditAnywhere, Category="World")
	FIntVector RenderFromBlock = FIntVector::NoneValue;

	UPROPERTY(EditAnywhere, Category="World")
	FIntVector RenderToBlock = FIntVector::NoneValue;

	UPROPERTY(EditAnywhere, Category="World")
	UMaterial* ChunkMaterial;

	UPROPERTY(EditAnywhere, Category="Noise")
	float Frequency = 0.03f;

	UPROPERTY(EditAnywhere, Category="Noise")
	int Seed = 1138;

	UPROPERTY(EditAnywhere, Category="Geometry")
	TSubclassOf<UVoxuelDecoratorBevelBase> Bevel;

	#if WITH_EDITORONLY_DATA
		UPROPERTY(EditAnywhere, Category="Debug")
		bool RenderSurfaceBoxes = false;

		UPROPERTY(EditAnywhere, Category="Debug")
		bool RenderSidePoints = false;
	#endif
	
protected:
	UFUNCTION(CallInEditor, Category = "World")
	void RenderAll();

	UFUNCTION(CallInEditor, Category = "World")
	void RenderChunk();

	UFUNCTION(CallInEditor, Category = "World")
	void Clear();

private:
	FIntVector Current = FIntVector::ZeroValue;
	
	bool RenderEnvironmentInitialized = false;

	TArray<TObjectPtr<AVoxuelChunkBase>> Chunks;

	void InitializeRenderEnvironment();
	bool DoRenderChunk(int y, int x);

	void DoRenderAll();
};
