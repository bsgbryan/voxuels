// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KismetProceduralMeshLibrary.h"
#include "VoxuelChunkGeometry.generated.h"

namespace Block {
	enum class Face;
}

class UVoxuelDecoratorBevelBase;

USTRUCT(BlueprintType)
struct FBevelConfig {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bevel")
	bool Apply = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Bevel")
	TArray<FVector> GenerateOffsets = {
		FVector::ZeroVector,
		FVector::ZeroVector,
		FVector::ZeroVector,
		FVector::ZeroVector
	};
};

UCLASS()
class VOXUELS_API UVoxuelChunkGeometry final : public UObject {
	GENERATED_BODY()

public:
	UVoxuelChunkGeometry();
	
	TArray<int>							 Triangles;
	TArray<FVector>					 Vertices;
	TArray<FVector>					 Normals;
	TArray<FVector2D>				 UVs;
	TArray<FProcMeshTangent> Tangents;
	TArray<FColor>					 Colors;

	FIntVector3 Dimensions;

	int VertexCount = 0;

	void Add(
		Block::Face face,
		const FVector& position,
		const UVoxuelDecoratorBevelBase* bevel,
		const FIntVector3& extent,
		const FIntVector3& dimensions = FIntVector3(0, 0, 0)
	);

	static TArray<FVector> GenerateQuad(
		Block::Face face,
		const FVector& position,
		const FIntVector3& dimensions = FIntVector3(0, 0, 0)
	);
};
