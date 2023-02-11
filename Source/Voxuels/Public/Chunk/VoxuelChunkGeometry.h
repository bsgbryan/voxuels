﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VoxuelChunkGeometry.generated.h"

namespace Block {
	enum class Face;
}

UCLASS()
class VOXUELS_API UVoxuelChunkGeometry final : public UObject {
	GENERATED_BODY()

public:
	UVoxuelChunkGeometry();
	
	TArray<int>		  Triangles;
	TArray<FVector>   Vertices;
	TArray<FVector>	  Normals;
	TArray<FVector2D> UVs;

	int VertexCount = 0;

	void Add(
		Block::Face face,
		const FVector position,
		const FIntVector3 dimensions = FIntVector3(1, 1, 1)
	);

	static TArray<FVector> GenerateQuad(
		Block::Face face,
		const FVector position,
		const FIntVector3 dimensions = FIntVector3(1, 1, 1)
	);
};
