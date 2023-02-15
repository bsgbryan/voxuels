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
		for (int i = 0; i < size.X * size.Y;) {
			int  _depth = 0;
			int  _width	= 0;
			
			while (_width + 1 + y < size.Y && surface[GetBlockMeshIndex(size, FVector(x, y + _width, z))])
				++_width;

			bool _depth_limit_reached = false;
			
			while (_width > 0 && !_depth_limit_reached && _depth + x < size.X) {
				for (int d = 0; d <= _width; d++)
					if (!surface[GetBlockMeshIndex(size, FVector(x + _depth, y + d, z))]) {
						_depth_limit_reached = true;
				
						break;
					}

				if (!_depth_limit_reached)
					++_depth;
			}

			if (_width > 0 || _depth > 0)
				geometry->Add(
					Block::Face::Up,
					FVector(x, y, z),
					FIntVector3(_depth == 0 ? 0 : _depth - 1, _width == 0 ? 0 : _width - 1, 0)
				);
			
			y += _width > 0 ? _width : 1;

			if (y == size.Y) {
				x += _depth > 0 ? _depth : 1;
				
				y = 0;
			}

			i += _width * _depth == 0 ?
				_width + _depth == 0 ?
					1
					:
					_width + _depth
				:
				_width * _depth;
		}

		z--;
		x = 0;
		y = 0;
	}
}

