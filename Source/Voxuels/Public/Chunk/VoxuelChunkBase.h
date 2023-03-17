// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Helpers.h"
#include "GameFramework/Actor.h"
#include "VoxuelChunkBase.generated.h"

class UVoxuelChunkGeometry;
class UFastNoiseWrapper;
class UProceduralMeshComponent;
class UVoxuelDecoratorBevelBase;

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

	TObjectPtr<UProceduralMeshComponent> Mesh;

	void Generate(UVoxuelDecoratorBevelBase* bevel);

protected:
	TObjectPtr<UVoxuelChunkGeometry> Geometry;
	TArray<uint8> Surface;

	uint8 Min = UINT8_MAX;
	uint8 Max = 0;

	virtual void GenerateMesh(UVoxuelDecoratorBevelBase* bevel);

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
