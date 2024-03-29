﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk/VoxuelChunkBase.h"
#include "VoxuelChunkCubeGreedy.generated.h"

namespace Block {
	enum class Face;
	enum class Type;
}

class UVoxuelDecoratorBevelBase;

UCLASS()
class VOXUELS_API AVoxuelChunkSquareGreedy final : public AVoxuelChunkBase {
	GENERATED_BODY()
	
public:
	virtual void GenerateMesh(UVoxuelDecoratorBevelBase* bevel) override;

private:
	int8 ProcessBlockForWidthSurface(
		const Block::Face direction,
		const FVector& position,
		const uint8 current_surface_size,
		const UVoxuelDecoratorBevelBase* bevel
	);

	int8 ProcessBlockForDepthSurface(
		const Block::Face direction,
		const FVector& position,
		const uint8 current_surface_size,
		const UVoxuelDecoratorBevelBase* bevel
	);

	uint16 ProcessDepthSurfaces(
		const FVector& position,
		const uint8 width,
		const uint16 depth,
		const UVoxuelDecoratorBevelBase* bevel
	);

	int8 ProcessRowForHeightSurface(
		const Block::Face direction,
		const FVector& position,
		const uint8 width,
		const uint8 depth,
		const UVoxuelDecoratorBevelBase* bevel
	);
};

