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
	int z = size.Z;

	while (--z > -1) {
		TArray<int8> _faces = TArray<int8>();
		_faces.SetNum(size.X * size.Y);
		
		for (int i = 0; i < size.X * size.Y;) {
			int _width = 0;
			int _back_wall  = 0;
			int _front_wall = 0;
			int _left_wall  = 0;
			int _right_wall = 0;
			
			while (
				x < size.X																							&&
				y + _width < size.Y																			&&
				(_faces[(y + _width) + (size.Y * x)] & Processed) == NA &&
				surface[GetBlockMeshIndex(size, FVector(x, y + _width, z))]
			) {
				const int _current = (y + _width) + (size.Y * x);
				_faces[_current] |= Processed;

				if (!surface[GetBlockMeshIndex(size, FVector(x + 1, y + _width, z))]) {
					++_front_wall;
					
					_faces[_current] |= Front;
				}
				else if (_front_wall > 0) {
					geometry->Add(
						Block::Face::Front,
						FVector(x, y + _width - _front_wall, z),
						FIntVector3(0, _front_wall, 0)
					);
					
					_front_wall = 0;
				}
				
				if (x > 0) {
					if (!surface[GetBlockMeshIndex(size, FVector(x - 1, y + _width, z))]) {
						++_back_wall;
				
						_faces[_current] |= Back;
					}
					else if (_back_wall > 0) {
						geometry->Add(
							Block::Face::Back,
							FVector(x, y + _width - _back_wall, z),
							FIntVector3(0, _back_wall - 1, 0)
						);
				
						_back_wall = 0;
					}	
				}
				
				++_width;
			}

			if (_back_wall > 0) {
				geometry->Add(
					Block::Face::Back,
					FVector(x, y + _width - _back_wall, z),
					FIntVector3(0, _back_wall - 1, 0)
				);
			}

			if (_width > 0) {
				if (!surface[GetBlockMeshIndex(size, FVector(x, y - 1, z))]) {
					++_left_wall;
					_faces[y + (size.Y * x)] |= Left;
				}
				else if (_left_wall > 0) {
					geometry->Add(
						Block::Face::Left,
						FVector(x - _left_wall, y, z),
						FIntVector3( _left_wall - 1, 0, 0)
					);
					
					_left_wall = 0;
				}

				if (!surface[GetBlockMeshIndex(size, FVector(x, y + _width, z))]) {
					++_right_wall;
					_faces[(y + _width - 1) + (size.Y * x)] |= Right;
				}
				else if (_right_wall > 0) {
					geometry->Add(
						Block::Face::Right,
						FVector(x - (_right_wall - 1), y + _width - 1, z),
						FIntVector3( _right_wall - 1, 0, 0)
					);
					
					_right_wall = 0;
				}
			}

			if (x == size.X - 1) {
				if (_left_wall > 0)
					geometry->Add(
						Block::Face::Left,
						FVector(x, y, z)
					);

				if (_right_wall > 0)
					geometry->Add(
						Block::Face::Right,
						FVector(x, y + _width - 1, z)
					);
			}

			int _depth = 1;
			bool _depth_limit_reached = false;
			
			while (_width > 0 && !_depth_limit_reached && _depth + x < size.X) {
				for (int d = 0; d < _width; d++) {
					if (!surface[GetBlockMeshIndex(size, FVector(x + _depth, y + d, z))]) {
						_depth_limit_reached = true;

						if (_width == 1) {
							_faces[y + (size.Y * (x + _depth))] &= ~Processed;
							geometry->Add(
								Block::Face::Front,
								FVector(x + _depth - 1, y, z),
								FIntVector3(0, 0, 0)
							);
						}
						else {
							for (int j = 0; j < d; j++)
								_faces[(y + j) + (size.Y * (x + _depth))] &= ~Processed;

							for (int f = 0; f <= _width; f++) {
								if (!surface[GetBlockMeshIndex(size, FVector(x + _depth, y + f, z))])
									++_front_wall;
								else {
									geometry->Add(
										Block::Face::Front,
										FVector(x + _depth - 1, y, z),
										FIntVector3(0, _front_wall, 0)
									);

									_front_wall = 0;
								}
							}
						}

						_front_wall = 0;
				
						break;
					}

					_faces[(y + d) + (size.Y * (x + _depth))] |= Processed;
				}

				if (!surface[GetBlockMeshIndex(size, FVector(x + _depth, y - 1, z))]) {
					++_left_wall;
					_faces[y + (size.Y * (x + _depth))] |= Left;
				}
				else if (_left_wall > 0) {
					geometry->Add(
						Block::Face::Left,
						FVector(x + _depth - _left_wall, y, z),
						FIntVector3(_left_wall - 1, 0, 0)
					);
				
					_left_wall = 0;
				}

				if (!surface[GetBlockMeshIndex(size, FVector(x + _depth, y + _width, z))]) {
					++_right_wall;
					_faces[(y + _width - 1) + (size.Y * (x + _depth))] |= Right;
				}
				else if (_right_wall > 0) {
					geometry->Add(
						Block::Face::Right,
						FVector(x + _depth - _right_wall, y + _width - 1, z),
						FIntVector3(_right_wall - 1, 0, 0)
					);
				
					_right_wall = 0;
				}

				if (!_depth_limit_reached)
					++_depth;
			}

			if (x + _depth < size.X) {
				geometry->Add(
					Block::Face::Front,
					FVector(x + _depth - 1, y, z),
					FIntVector3(0, _width - 1, 0)
				);
			}
			
			if (--_left_wall > 0) {
				if (x + _depth == size.X)
					geometry->Add(
						Block::Face::Left,
						FVector(x + _depth - _left_wall - 1, y, z),
						FIntVector3(_left_wall, 0, 0)
					);
				else
					geometry->Add(
						Block::Face::Left,
						FVector(x + _depth - _left_wall, y, z),
						FIntVector3(_left_wall - 1, 0, 0)
					);
			}
			
			if (--_right_wall > 0) {
				if (x + _depth == size.X)
					geometry->Add(
						Block::Face::Right,
						FVector(x + _depth - _right_wall - 1, y + _width - 1, z),
						FIntVector3(_right_wall, 0, 0)
					);
				else
					geometry->Add(
						Block::Face::Right,
						FVector(x + _depth - _right_wall, y + _width - 1, z),
						FIntVector3(_right_wall - 1, 0, 0)
					);
			}

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

			if (x == size.X)
				x = 0;
		}
	}
}

