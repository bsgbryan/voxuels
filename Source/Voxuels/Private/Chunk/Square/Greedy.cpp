// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "Chunk/Square/VoxuelChunkSquareGreedy.h"
#include "Chunk/VoxuelChunkGeometry.h"

void AVoxuelChunkSquareGreedy::GenerateMesh(
	const TObjectPtr<UVoxuelChunkGeometry> geometry,
	TArray<bool> surface,
	const FIntVector size
) const {
	int x = 0;
	int y = 0;
	int z = size.Z - 1;

	while (z > -1) {
		for (int i = 0; i < size.X * size.Y; i++) {
			int  _depth		 = 0;
			int  _width		 = 0;
			auto _width_test = FVector(x, y, z);
			
			while (_width + x < size.X - 1 && surface[GetBlockMeshIndex(size, _width_test)]) {
				i++;
				_width++;
				_width_test = FVector(x + _width, y, z);

				for (int d = 0; d < _width; d++) {
					auto _depth_test = FVector(x + d, y + ++_depth, z);

					if (!surface[GetBlockMeshIndex(size, _depth_test)])
						_width = --_depth;
				}
			}

			if (_width > 0) {
				geometry->Add(Block::Face::Up, FVector(x, y, z), FIntVector3(_width - 1, _depth - 1, 0));

				x += _width;
			}
			else
				x++;

			if (_depth > 0)
				y += _depth;

			if (x == size.X) {
				x = 0;
				y++;
			}
		}

		z--;
		x = 0;
		y = 0;
	}
}

