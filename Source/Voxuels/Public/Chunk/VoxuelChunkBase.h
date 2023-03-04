// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers.h"
#include "GameFramework/Actor.h"
#include "VoxuelChunkBase.generated.h"

class UVoxuelChunkGeometry;
class UFastNoiseWrapper;
class UProceduralMeshComponent;

UCLASS()
class VOXUELS_API AVoxuelChunkBase : public AActor {
	GENERATED_BODY()
	
public:	
	AVoxuelChunkBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
	TObjectPtr<UFastNoiseWrapper> Noise;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
	bool Threaded = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
	FIntVector Dimensions = FIntVector(32, 32, 32);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
	FRenderExtent Window;

	void Generate();

protected:
	TObjectPtr<UVoxuelChunkGeometry> Geometry;
	TObjectPtr<UProceduralMeshComponent> Mesh;
	TArray<uint8> Surface;

	virtual void GenerateMesh();

	void GenerateSurface();

	int GetBlockIndex(const FVector& position) const;
	int GetBlockMeshIndex(const FVector& position) const;

private:	
	bool ProcessNeighbor(
		const bool width,
		const FVector2D& position,
		const uint8 value,
		const uint8 offset
	);
};
