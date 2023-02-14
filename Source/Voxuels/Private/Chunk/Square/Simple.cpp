// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk/Square/VoxuelChunkSquareSimple.h"
#include "Block.h"
#include "Chunk/VoxuelChunkGeometry.h"

void AVoxuelChunkSquareSimple::GenerateMesh(
	const TObjectPtr<UVoxuelChunkGeometry> geometry,
	TArray<bool> surface,
	const FIntVector size
) const {
	for (int i = 0, x = -1, z = -1; i < size.X * size.Y * size.Z; i++) {
		const int y = i % size.Y;

		if (i % (size.X * size.Y) == 0)
			x = 0;
		else
			x = i % size.X == 0 ? x + 1 : x;
		
		z = i % (size.X * size.Y) == 0 ? z + 1 : z;

		if (const auto position = FVector(x, y, z); surface[GetBlockMeshIndex(size, position)])
			for (const auto face : Block::AllFaces)
				if (const auto _neighbor = position + Block::Vectors[static_cast<int>(face)]; surface[GetBlockMeshIndex(size, _neighbor)] == false)
					geometry->Add(face, position);
	}
}
