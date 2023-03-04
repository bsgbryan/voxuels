// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "VoxuelsLogger.h"
#include "Chunk/Square/VoxuelChunkSquareGreedy.h"
#include "Chunk/VoxuelChunkGeometry.h"

void AVoxuelChunkSquareGreedy::GenerateMesh() {
	TArray<bool> _processed;
	_processed.SetNum(Dimensions.Y * Dimensions.X * Dimensions.Z);
	
	for (int16 i = 0, x = 0, y = 0, z = Dimensions.Z - 1; i < Dimensions.Y * Dimensions.X;) {
		uint8 _width							 = 0;
		uint8 _back_surface_width  = 0;
		uint8 _front_surface_width = 0;

		uint8 _y			=  y + _width;
		int		_index = GetBlockMeshIndex(FVector(x, _y, z));

		while (
			 x < Dimensions.X												 &&
			_y < Dimensions.Y												 &&
			Surface[_index] & Block::Surface::Exists &&
			!_processed[_y + (Dimensions.Y * x)]
		) {
			_back_surface_width += ProcessBlockForWidthSurface(
				Block::Face::Back,
				FVector(x, y + _width, z),
				_back_surface_width
			);

			_front_surface_width += ProcessBlockForWidthSurface(
				Block::Face::Front,
				FVector(x - 1, y + _width, z),
				_front_surface_width
			);

			_y		 =  y + ++_width;
			_index = GetBlockMeshIndex(FVector(x, _y, z));
		}

		if (_back_surface_width)
			RenderWidthSurface(
				Block::Face::Back,
				_back_surface_width,
				FVector(x, y + _width - _back_surface_width, z)
			);

		if (_front_surface_width)
			RenderWidthSurface(
				Block::Face::Front,
				_front_surface_width,
				FVector(x - 1, y + _width - _front_surface_width, z)
			);

		if (_width > 0) {
			uint8 _left_surface_size  = 0;
			uint8 _right_surface_size = 0;
			
			uint8 _depth 							 = 1;
			bool  _depth_limit_reached = false;
			
			const uint16 _result = ProcessDepthSurfaces(
					FVector(x, y, z),
					_width - 1,
					(_left_surface_size << 8) | _right_surface_size,
					_processed
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
						_processed[(y + w) + (Dimensions.Y * (x + _depth))] = true;
					
					const uint16 __result = ProcessDepthSurfaces(
						FVector(x + _depth, y, z),
						_width - 1,
						(_left_surface_size << 8) | _right_surface_size,
						_processed
					);

					_left_surface_size  += static_cast<int8>(__result >> 8);
					_right_surface_size += static_cast<int8>(__result);
				
					++_depth;
				}
				else {
					uint8 _front_surface_size = 0;

					for (uint8 f = 0; f < _width; f++)
						_front_surface_size += ProcessBlockForWidthSurface(
							Block::Face::Front,
							FVector(x + _depth - 1, y + f, z),
							_front_surface_size
						);

					if (_front_surface_size)
						RenderWidthSurface(
							Block::Face::Front,
							_front_surface_size,
							FVector(x + _depth - 1, y + _width - _front_surface_size, z)
						);
				}
			}
		
			if (_left_surface_size)
				RenderDepthSurface(
					Block::Face::Left,
					_left_surface_size,
					FVector(x + _depth - _left_surface_size, y, z)
				);

			if (_right_surface_size)
				RenderDepthSurface(
					Block::Face::Right,
					_right_surface_size,
					FVector(x + _depth - _right_surface_size, y + _width - 1, z)
				);

			if (_depth + x == Dimensions.X) {
				uint8 _front_surface_size = 0;

				for (uint8 f = 0; f < _width; f++)
					_front_surface_size = ProcessBlockForWidthSurface(
							Block::Face::Front,
							FVector(x + _depth - 1, y + f, z),
							_front_surface_size
						);

				if (_front_surface_size)
					RenderWidthSurface(
						Block::Face::Front,
						_front_surface_size,
						FVector(x + _depth - 1, y + _width - _front_surface_size, z)
					);
			}
			
			if (Surface[GetBlockMeshIndex(FVector(x, y, z))] & Block::Surface::Up)
				Geometry->Add(
					Block::Face::Up,
					FVector(x, y, z),
					FIntVector3(_depth == 0 ? 0 : _depth - 1, _width == 0 ? 0 : _width - 1, 0)
				); 
		}

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

			if (z > -1)
				i = 0;
		}
	}
}

void AVoxuelChunkSquareGreedy::RenderWidthSurface(
	const Block::Face direction,
	const uint8 size,
	const FVector& position
) const {
	Geometry->Add(
		direction,
		position,
		FIntVector3(0, size - 1, 0)
	);
}

void AVoxuelChunkSquareGreedy::RenderDepthSurface(
	const Block::Face direction,
	const uint8 size,
	const FVector& position
) const {
	Geometry->Add(
		direction,
		position,
		FIntVector3(size - 1, 0, 0)
	);
}

int8 AVoxuelChunkSquareGreedy::ProcessBlockForWidthSurface(
	const Block::Face direction,
	const FVector& position,
	const uint8 current_surface_size
) {
	if (
		const uint8	_value = Surface[GetBlockMeshIndex(position)];
		_value & (direction == Block::Face::Front ? Block::Surface::Front : Block::Surface::Back)
	)
		return 1;

	if (current_surface_size) {
		const float		_y = position.Y - current_surface_size;
		const FVector _p = FVector(position.X, _y, position.Z);
		
		RenderWidthSurface(direction, current_surface_size, _p);

		return -current_surface_size;
	}

	return 0;
}

int8 AVoxuelChunkSquareGreedy::ProcessBlockForDepthSurface(
	const Block::Face direction,
	const FVector& position,
	const uint8 current_surface_size,
	TArray<bool> processed
) {
	processed[position.Y + (Dimensions.Y * position.X)] = true;
	
	if (
		const uint8 _value = Surface[GetBlockMeshIndex(position)];
		_value & (direction == Block::Face::Left ? Block::Surface::Left : Block::Surface::Right)
	)
		return 1;
	
	if (current_surface_size) {
		const float		_x = position.X - current_surface_size;
		const FVector _p = FVector(_x, position.Y, position.Z);
		
		RenderDepthSurface(direction, current_surface_size, _p);

		return -current_surface_size;
	}

	return 0;
}

uint16 AVoxuelChunkSquareGreedy::ProcessDepthSurfaces(
	const FVector& position,
	const uint8 width,
	const uint16 depth,
	const TArray<bool> processed
) {	
	return (ProcessBlockForDepthSurface(
		Block::Face::Left,
		position,
		static_cast<uint8>(depth >> 8),
		processed
		) << 8) | (ProcessBlockForDepthSurface(
			Block::Face::Right,
			FVector(position.X, position.Y + width, position.Z),
			static_cast<uint8>(depth),
			processed
		) & 0xff);
}
