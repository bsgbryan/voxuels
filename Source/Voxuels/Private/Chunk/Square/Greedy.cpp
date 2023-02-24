// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "VoxuelsLogger.h"
#include "Chunk/Square/VoxuelChunkSquareGreedy.h"
#include "Chunk/VoxuelChunkGeometry.h"

enum OpenSide : uint8 {
	Processed = 1 << 1,
	Front 		= 1 << 2,
	Back  		= 1 << 3,
	Left  		= 1 << 4,
	Right 		= 1 << 5,
	Up				= 1 << 6,
	Down  		= 1 << 7
};

void AVoxuelChunkSquareGreedy::GenerateMesh(
	const TObjectPtr<UVoxuelChunkGeometry> geometry,
	const TArray<bool> surface,
	const FIntVector size
) const {
	uint8 x = 0;
	uint8 y = 0;
	uint8 z = size.Z;

	while (--z > -1) {
		TArray<int8> _faces = TArray<int8>();
		_faces.SetNum(size.X * size.Y);
		
		for (int i = 0; i < size.X * size.Y;) {
			uint8 _width = 0;

#pragma region Width Processing
			uint8 _back_surface_width = 0;
			
			while (
				x < size.X																						 &&
				y + _width < size.Y																		 &&
				(_faces[(y + _width) + (size.Y * x)] & Processed) == 0 &&
				surface[GetBlockMeshIndex(size, FVector(x, y + _width, z))]
			) {
				_faces[(y + _width) + (size.Y * x)] |= Processed;

				// This builds up the Surface size, and renders the Back Surface if
				// a gap is encountered
				_back_surface_width = ProcessBlockForWidthSurface(
					Block::Face::Back,
					surface,
					geometry,
					size,
					FVector(x, y + _width, z),
					_back_surface_width
				);
				
				++_width;
			}

			// Render the Back Surface built up after any gap encountered above,
			// if necessary
			if (_back_surface_width)
				RenderWidthSurface(
					Block::Face::Back,
					_back_surface_width,
					FVector(x, y + _width - _back_surface_width, z),
					geometry
				);
#pragma endregion

#pragma region Depth Processing
			if (_width > 0) {
				uint8 _left_surface_size  = 0;
				uint8 _right_surface_size = 0;
				
				uint8 _depth 							 = 1;
				bool  _depth_limit_reached = false;
				
				const uint16 _result = ProcessDepthSurfaces(
						surface,
						geometry,
						size,
						FVector(x + _depth, y, z),
						_width - 1,
						(_left_surface_size << 8) | _right_surface_size
					);

				_left_surface_size  += static_cast<int8>(_result >> 8);
				_right_surface_size += static_cast<int8>(_result);

				while (!_depth_limit_reached && _depth + x < size.X) {
					for (int d = 0; d < _width; d++) {
						if (!surface[GetBlockMeshIndex(size, FVector(x + _depth, y + d, z))]) {
							_depth_limit_reached = true;

							for (int j = 0; j <= d; j++)
								_faces[(y + j) + (size.Y * (x + _depth))] &= ~Processed;
				
							break;
						}

						_faces[(y + d) + (size.Y * (x + _depth))] |= Processed;
					}

					if (!_depth_limit_reached) {
						const uint16 _result = ProcessDepthSurfaces(
							surface,
							geometry,
							size,
							FVector(x + _depth, y, z),
							_width - 1,
							(_left_surface_size << 8) | _right_surface_size
						);

						_left_surface_size  += static_cast<uint8>(_result >> 8);
						_right_surface_size += static_cast<uint8>(_result);
					
						++_depth;
					}
					else {
						uint8 _front_surface_size = 0;

						for (uint8 f = 0; f <= _width; f++)
							_front_surface_size = ProcessBlockForWidthSurface(
								Block::Face::Front,
								surface,
								geometry,
								size,
								FVector(x + _depth - 1, y + f, z),
								_front_surface_size
							);

						if (_front_surface_size)
							RenderWidthSurface(
								Block::Face::Front,
								_front_surface_size,
								FVector(x + _depth - 1, y + _width, z),
								geometry
							);
					}
				}
			
				if (_left_surface_size)
					RenderDepthSurface(
						Block::Face::Left,
						_left_surface_size,
						FVector(x + _depth - _left_surface_size, y, z),
						geometry
					);

				if (_right_surface_size)
					RenderDepthSurface(
						Block::Face::Right,
						_right_surface_size,
						FVector(x + _depth - _right_surface_size, y, z),
						geometry
					);

#pragma region Render Top Surface
				if (_depth - 1 > 0)
					geometry->Add(
						Block::Face::Up,
						FVector(x, y, z),
						FIntVector3(_depth == 0 ? 0 : _depth - 1, _width == 0 ? 0 : _width - 1, 0)
					);
#pragma endregion 
			}
#pragma endregion 

#pragma region Index Management
			y += _width > 0 ? _width : 1;
			i += _width > 0 ? _width : 1;

			if (y == size.Y) {
				++x;
				
				y = 0;
			}

			if (x == size.X)
				x = 0;
#pragma endregion 
		}
	}
}

void AVoxuelChunkSquareGreedy::RenderWidthSurface(
	const Block::Face direction,
	const uint8 size,
	const FVector position,
	const TObjectPtr<UVoxuelChunkGeometry> geometry
) {
	geometry->Add(
		direction,
		position,
		FIntVector3(0, size - 1, 0)
	);
}

void AVoxuelChunkSquareGreedy::RenderDepthSurface(
	const Block::Face direction,
	const uint8 size,
	const FVector position,
	const TObjectPtr<UVoxuelChunkGeometry> geometry
) {
	geometry->Add(
		direction,
		position,
		FIntVector3(size - 1, 0, 0)
	);
}

int8 AVoxuelChunkSquareGreedy::ProcessBlockForWidthSurface(
	const Block::Face direction,
	const TArray<bool> surface,
	const TObjectPtr<UVoxuelChunkGeometry> geometry,
	const FIntVector size,
	const FVector position,
	const uint8 current_surface_size
) {
	const float _x = position.X + (direction == Block::Face::Front ? 1 : -1);

	if (const FVector _test = FVector(_x, position.Y, position.Z); !surface[GetBlockMeshIndex(size, _test)])
		return 1;

	if (current_surface_size) {
		const float		_y = position.Y - current_surface_size;
		const FVector _p = FVector(position.X, _y, position.Z);
		
		RenderWidthSurface(direction, current_surface_size, _p, geometry);

		return -current_surface_size;
	}

	return 0;
}

int8 AVoxuelChunkSquareGreedy::ProcessBlockForDepthSurface(
	const Block::Face direction,
	const TArray<bool> surface,
	const TObjectPtr<UVoxuelChunkGeometry> geometry,
	const FIntVector size,
	const FVector position,
	const uint8 current_surface_size
) {
	const float _y = position.Y + (direction == Block::Face::Right ? 1 : -1);
	
	if (const FVector _test = FVector(position.X, _y, position.Z); !surface[GetBlockMeshIndex(size, _test)])
		return 1;
	
	if (current_surface_size) {
		const float		_x = position.X - current_surface_size;
		const FVector _p = FVector(_x, position.Y, position.Z);
		
		RenderDepthSurface(Block::Face::Left, current_surface_size, _p, geometry);

		return -current_surface_size;
	}

	return 0;
}

uint16 AVoxuelChunkSquareGreedy::ProcessDepthSurfaces(
	const TArray<bool> surface,
	const TObjectPtr<UVoxuelChunkGeometry> geometry,
	const FIntVector size,
	const FVector position,
	const uint8 width,
	const uint16 depth
) {
	return (ProcessBlockForDepthSurface(
		Block::Face::Left,
		surface,
		geometry,
		size,
		position,
		static_cast<uint8>(depth >> 8)
		) << 8) | (ProcessBlockForDepthSurface(
			Block::Face::Right,
			surface,
			geometry,
			size,
			FVector(position.X, position.Y + width, position.Z),
			static_cast<uint8>(depth)
		) & 0xff);
}
