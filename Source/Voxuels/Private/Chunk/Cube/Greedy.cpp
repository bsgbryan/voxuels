﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "VoxuelsLogger.h"
#include "Chunk/Cube/VoxuelChunkCubeGreedy.h"
#include "Chunk/VoxuelChunkGeometry.h"

void AVoxuelChunkSquareGreedy::GenerateMesh(UVoxuelDecoratorBevelBase* bevel) {
	TArray<bool> _processed;
	_processed.SetNum(Dimensions.Y * Dimensions.X * Dimensions.Z);
	
	for (int16 i = 0, x = 0, y = 0, z = Max; i < Dimensions.Y * Dimensions.X;) {
		#pragma region Process Y (width) Axis
			uint8 _width							 = 0;
			uint8 _top_surface_width	 = 0;
			uint8 _back_surface_width  = 0;
			uint8 _front_surface_width = 0;

			uint8 _y			=  y + _width;
			int		_index = GetBlockMeshIndex(FVector(x, _y, z));

			while (
				  x < Dimensions.X												 &&
				 _y < Dimensions.Y												 &&
				(Surface[_index] & Block::Surface::Exists) &&
				!_processed[_y + (Dimensions.Y * x) + (z * (Dimensions.Y * Dimensions.X))]
			) {
				_back_surface_width += ProcessBlockForWidthSurface(
					Block::Face::Back,
					FVector(x, y + _width, z),
					_back_surface_width,
					bevel
				);

				_front_surface_width += ProcessBlockForWidthSurface(
					Block::Face::Front,
					FVector(x - 1, y + _width, z),
					_front_surface_width,
					bevel
				);

				_processed[_y + (Dimensions.Y * x) + (z * (Dimensions.Y * Dimensions.X))] = true;

				_y		 =  y + ++_width;
				_index = GetBlockMeshIndex(FVector(x, _y, z));

				_top_surface_width = _width;
			}

			if (_back_surface_width)
				Geometry->Add(
					Block::Face::Back,
					FVector(x, y + _width - _back_surface_width, z),
					bevel,
					Dimensions,
					FIntVector3(0, _back_surface_width - 1, 0)
				);

			if (_front_surface_width)
				Geometry->Add(
					Block::Face::Front,
					FVector(x - 1, y + _width - _front_surface_width, z),
					bevel,
					Dimensions,
					FIntVector3(0, _front_surface_width - 1, 0)
				);
		#pragma endregion 

		#pragma region Process the X (depth) Axis
			if (_width > 0) {
				uint8 _top_surface_depth  = 1;
				uint8 _left_surface_size  = 0;
				uint8 _right_surface_size = 0;
				
				uint8 _depth 							 = 1;
				bool  _depth_limit_reached = false;
				
				const uint16 _result = ProcessDepthSurfaces(
					FVector(x, y, z),
					_width - 1,
					(_left_surface_size << 8) | _right_surface_size,
					bevel
				);

				_left_surface_size  += static_cast<int8>(_result >> 8);
				_right_surface_size += static_cast<int8>(_result);
				
				while (
					!_depth_limit_reached &&
					_depth + x < Dimensions.X
				) {
					for (int d = 0; d < _width; d++)
						if (!(Surface[GetBlockMeshIndex(FVector(x + _depth, y + d, z))] & Block::Surface::Exists)) {
							_depth_limit_reached = true;
				
							break;
						}

					if (!_depth_limit_reached) {
						for (uint8 w = 0; w < _width; w++)
							_processed[(y + w) + (Dimensions.Y * (x + _depth)) + (z * (Dimensions.Y * Dimensions.X))] = true;
						
						const uint16 __result = ProcessDepthSurfaces(
							FVector(x + _depth, y, z),
							_width - 1,
							(_left_surface_size << 8) | _right_surface_size,
							bevel
						);

						_left_surface_size  += static_cast<int8>(__result >> 8);
						_right_surface_size += static_cast<int8>(__result);

						_top_surface_depth += ProcessRowForHeightSurface(
							Block::Face::Up,
							FVector(x + _depth, y, z),
							_top_surface_width,
							_top_surface_depth,
							bevel
						);
					
						++_depth;
					}
					else {
						uint8 _front_surface_size = 0;

						for (uint8 f = 0; f < _width; f++)
							_front_surface_size += ProcessBlockForWidthSurface(
								Block::Face::Front,
								FVector(x + _depth - 1, y + f, z),
								_front_surface_size,
								bevel
							);

						if (_front_surface_size)
							Geometry->Add(
								Block::Face::Front,
								FVector(x + _depth - 1, y + _width - _front_surface_size, z),
								bevel,
								Dimensions,
								FIntVector3(0, _front_surface_size - 1, 0)
							);
					}
				}
			
				if (_left_surface_size)
					Geometry->Add(
						Block::Face::Left,
						FVector(x + _depth - _left_surface_size, y, z),
						bevel,
						Dimensions,
						FIntVector3(_left_surface_size - 1, 0, 0)
					);

				if (_right_surface_size)
					Geometry->Add(
						Block::Face::Right,
						FVector(x + _depth - _right_surface_size, y + _width - 1, z),
						bevel,
						Dimensions,
						FIntVector3(_right_surface_size - 1, 0, 0)
					);

				if (_top_surface_width && _top_surface_depth)
					Geometry->Add(
						Block::Face::Up,
						FVector(x + _depth - _top_surface_depth, y + _width - _top_surface_width, z),
						bevel,
						Dimensions,
						FIntVector3(_top_surface_depth - 1, _top_surface_width - 1, 0)
					);

				if (_depth + x == Dimensions.X) {
					uint8 _front_surface_size = 0;

					for (uint8 f = 0; f < _width; f++)
						_front_surface_size += ProcessBlockForWidthSurface(
							Block::Face::Front,
							FVector(x + _depth - 1, y + f, z),
							_front_surface_size,
							bevel
						);

					if (_front_surface_size)
						Geometry->Add(
							Block::Face::Front,
							FVector(x + _depth - 1, y + _width - _front_surface_size, z),
							bevel,
							Dimensions,
							FIntVector3(0, _front_surface_size - 1, 0)
						);
				}
			#pragma endregion
		}

		#pragma region Index Management
			y += _width > 0 ? _width : 1;
			i += _width > 0 ? _width : 1;

			if (y == Dimensions.Y) {
				++x;
				
				y = 0;
			}

			if (x == Dimensions.X)
				x = 0;

			if (i > 0 && x == 0 && y == 0) {
				--z;

				if (z >= Min)
					i = 0;
			}
		#pragma endregion 
	}
}

int8 AVoxuelChunkSquareGreedy::ProcessBlockForWidthSurface(
	const Block::Face direction,
	const FVector& position,
	const uint8 current_surface_size,
	const UVoxuelDecoratorBevelBase* bevel
) {
	if (
		const uint8	_value = Surface[GetBlockMeshIndex(position)];
		_value & (direction == Block::Face::Front ? Block::Surface::Front : Block::Surface::Back)
	)
		return 1;

	if (current_surface_size) {
		const float		_y = position.Y - current_surface_size;
		const FVector _p = FVector(position.X, _y, position.Z);

		Geometry->Add(
			direction,
			_p,
			bevel,
			Dimensions,
			FIntVector3(0, current_surface_size - 1, 0)
		);

		return -current_surface_size;
	}

	return 0;
}

int8 AVoxuelChunkSquareGreedy::ProcessBlockForDepthSurface(
	const Block::Face direction,
	const FVector& position,
	const uint8 current_surface_size,
	const UVoxuelDecoratorBevelBase* bevel
) {
	if (
		const uint8 _value = Surface[GetBlockMeshIndex(position)];
		(_value & (direction == Block::Face::Left ? Block::Surface::Left : Block::Surface::Right)) != 0
	)
		return 1;

	if (current_surface_size) {
		const float		_x = position.X - current_surface_size;
		const FVector _p = FVector(_x, position.Y, position.Z);

		Geometry->Add(
			direction,
			_p,
			bevel,
			Dimensions,
			FIntVector3(current_surface_size - 1, 0, 0)
		);

		return -current_surface_size;
	}

	return 0;
}

uint16 AVoxuelChunkSquareGreedy::ProcessDepthSurfaces(
	const FVector& position,
	const uint8 width,
	const uint16 depth,
	const UVoxuelDecoratorBevelBase* bevel
) {	
	return (ProcessBlockForDepthSurface(
		Block::Face::Left,
		position,
		static_cast<uint8>(depth >> 8),
		bevel
		) << 8) | (ProcessBlockForDepthSurface(
			Block::Face::Right,
			FVector(position.X, position.Y + width, position.Z),
			static_cast<uint8>(depth),
			bevel
		) & 0xff);
}

int8 AVoxuelChunkSquareGreedy::ProcessRowForHeightSurface(
	const Block::Face direction,
	const FVector& position,
	const uint8 width,
	const uint8 depth,
	const UVoxuelDecoratorBevelBase* bevel
) {
	if (Surface[GetBlockMeshIndex(position)] & Block::Surface::Up)
		return 1;

	if (width || depth) {
		const float		_y = position.Y - (width - 1);
		const float   _x = position.X -  depth;
		const FVector _p = FVector(_x, _y, position.Z);

		Geometry->Add(
			direction,
			_p,
			bevel,
			Dimensions,
			FIntVector3(depth - 1, width - 1, 0)
		);

		return -depth;
	}

	return 0;
}