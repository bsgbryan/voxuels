// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxuelChunkBase.generated.h"

namespace Block {
	enum class Face;
	enum class Type;
}

class UVoxuelChunkGeometry;
class UFastNoiseWrapper;
class UProceduralMeshComponent;

UCLASS()
class VOXUELS_API AVoxuelChunkBase : public AActor {
	GENERATED_BODY()
	
public:	
	AVoxuelChunkBase();

	UFUNCTION(BlueprintCallable, Category="Chunk")
	void Generate(
		bool threaded,
		FIntVector size,
		int seed,
		float frequency
	) const;

protected:
	TObjectPtr<UProceduralMeshComponent> Mesh;
	TObjectPtr<UFastNoiseWrapper> Noise;

	static TArray<bool> GenerateSurface(
		FVector location,
		TObjectPtr<UFastNoiseWrapper> noise,
		FIntVector size,
		int seed,
		float frequency
	);

	virtual void GenerateMesh(
		TObjectPtr<UVoxuelChunkGeometry> geometry,
		TArray<bool> surface,
		FIntVector size
	) const;

	static int GetBlockIndex(
		FIntVector size,
		FVector position
	);

	static int GetBlockMeshIndex(
		const FIntVector size,
		const FVector position
	);
};
