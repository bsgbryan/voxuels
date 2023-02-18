// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "Chunk/Square/VoxuelChunkSquareGreedy.h"
#include "Chunk/VoxuelChunkGeometry.h"

enum OpenSide : int8 {
	NA				= 0,
	Processed = 2^0,
	Front 		= 2^1,
	Back  		= 2^2,
	Left  		= 2^3,
	Right 		= 2^4,
	Up				= 2^5,
	Down  		= 2^6
};

void AVoxuelChunkSquareGreedy::GenerateMesh(
	const TObjectPtr<UVoxuelChunkGeometry> geometry,
	TArray<bool> surface,
	const FIntVector size
) const {
	int x = 0;
	int y = 0;
	int z = size.Z - 1;

	while (z > -1) {
		TArray<int8> _faces = TArray<int8>();
		_faces.SetNum(size.X * size.Y);
		
		for (int i = 0; i < size.X * size.Y;) {
			int _width = 0;
			int _back_wall  = 0;
			int _front_wall = 0;
			int _left_wall  = 0;
			int _right_wall = 0;
			
			while (
				_width + y < size.Y																						&&
				x < size.X																										&&
				(_faces[(y + _width) + (size.Y * x)] & Processed) == NA &&
				surface[GetBlockMeshIndex(size, FVector(x, y + _width, z))]
			) {
				_faces[(y + _width) + (size.Y * x)] |= Processed;
				
				// if (x > 0) {
				// 	if (!surface[GetBlockMeshIndex(size, FVector(x - 1, y + _width, z))]) {
				// 		++_back_wall;
				//
				// 		_faces[(y + _width) + (size.Y * x)] |= Back;
				// 	}
				// 	else if (_back_wall > 0) {
				// 		geometry->Add(
				// 			Block::Face::Back,
				// 			FVector(x - 1, y + _width - _back_wall, z),
				// 			FIntVector3(0, _back_wall, 0)
				// 		);
				//
				// 		_back_wall = 0;
				// 	}	
				//
				// 	if (!surface[GetBlockMeshIndex(size, FVector(x + 1, y + _width, z))]) {
				// 		++_front_wall;
				//
				// 		_faces[(y + _width) + (size.Y * x)] |= Front;
				// 	}
				// 	else if (_front_wall > 0) {
				// 		geometry->Add(
				// 			Block::Face::Front,
				// 			FVector(x + 1, y + _width - _front_wall, z),
				// 			FIntVector3(0, _front_wall, 0)
				// 		);
				//
				// 		_front_wall = 0;
				// 	}
				// }
				
				++_width;
			}

			// if (--_back_wall > 0) {
			// 	geometry->Add(
			// 		Block::Face::Back,
			// 		FVector(x, y + _width - _back_wall - 1, z),
			// 		FIntVector3(0, _back_wall, 0)
			// 	);
			// }
			//
			// if (--_front_wall > 0) {
			// 	geometry->Add(
			// 		Block::Face::Front,
			// 		FVector(x, y + _width - _front_wall - 1, z),
			// 		FIntVector3(0, _front_wall, 0)
			// 	);
			// }

			int _depth = 1;
			bool _depth_limit_reached = false;
			
			while (_width > 0 && !_depth_limit_reached && _depth + x < size.X) {
				for (int d = 0; d < _width; d++) {
					if (!surface[GetBlockMeshIndex(size, FVector(x + _depth, y + d, z))]) {
						_depth_limit_reached = true;

						if (_width == 1)
							_faces[y + (size.Y * (x + _depth))] &= ~Processed;
						else
							for (int j = 0; j < d; j++)
								_faces[(y + j) + (size.Y * (x + _depth))] &= ~Processed;
				
						break;
					}

					_faces[(y + d) + (size.Y * (x + _depth))] |= Processed;
				}

				// if (!surface[GetBlockMeshIndex(size, FVector(x + _depth, y + _width + 1, z))]) {
				// 	++_right_wall;
				// 	_faces[(y + (_width - 1)) + (size.Y * (x + _depth))] |= Right;
				// }
				// else if (_right_wall > 0) {
				// 	geometry->Add(
				// 		Block::Face::Right,
				// 		FVector(x + (_depth > 0 ? _depth - 2 : 0) - (_right_wall - 1), y + _width - 1, z),
				// 		FIntVector3( _depth > 0 ? _right_wall : _right_wall - 1, 0, 0)
				// 	);

					_right_wall = 0;
				// }

				if (!_depth_limit_reached)
					++_depth;
			}

			// if (_right_wall > 0) {
			// 	geometry->Add(
			// 			Block::Face::Right,
			// 			FVector(x + (_depth > 0 ? _depth - 2 : 0) - (_right_wall - 1), y + _width - 1, z),
			// 			FIntVector3(_depth > 0 ? _right_wall : _right_wall - 1, 0, 0)
			// 		);
			// }

			if (_width > 0 || _depth - 1 > 0)
				geometry->Add(
					Block::Face::Up,
					FVector(x, y, z),
					FIntVector3(_depth == 0 ? 0 : _depth - 1, _width == 0 ? 0 : _width - 1, 0)
				);
			
			y += _width > 0 ? _width : 1;
			i += _width > 0 ? _width : 1;

			if (y == size.Y) {
				++x;
				
				y = 0;
			}
		}

		z--;
		x = 0;
		y = 0;
	}
}

