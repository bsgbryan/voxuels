// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "Chunk/VoxuelChunkBase.h"
#include "Chunk/VoxuelChunkGeometry.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseWrapper.h"

AVoxuelChunkBase::AVoxuelChunkBase() {
	Geometry = NewObject<UVoxuelChunkGeometry>();
 	Noise		 = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("Noise"));

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	Mesh->SetCastShadow(true);

	SetRootComponent(Mesh);
}

void AVoxuelChunkBase::Generate() {
	if (Threaded)
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [chunk = this] {
			chunk->GenerateSurface();
			chunk->GenerateMesh();

			AsyncTask(ENamedThreads::GameThread, [render = chunk] {
				render->Mesh->CreateMeshSection(
					0,
					render->Geometry->Vertices,
					render->Geometry->Triangles,
					render->Geometry->Normals,
					render->Geometry->UVs,
					TArray<FColor>(),
					render->Geometry->Tangents,
					false
				);
			});
		});
	else {
		GenerateSurface();
		GenerateMesh();

		Mesh->CreateMeshSection(
			0,
			Geometry->Vertices,
			Geometry->Triangles,
			Geometry->Normals,
			Geometry->UVs,
			TArray<FColor>(),
			Geometry->Tangents,
			false
		);
	}
}

void AVoxuelChunkBase::GenerateSurface() {
	Surface.SetNum((Dimensions.Y + 2) * (Dimensions.X + 2) * Dimensions.Z);

	for (int i = 0, x = -1, _width = Dimensions.Y + 2, _depth = Dimensions.X + 2; i < _width * _depth; i++) {
		const int y = i % _width;

		x = i % _width == 0 ? x + 1 : x;

		if (
			(Window.From.Y == INDEX_NONE && Window.From.X == INDEX_NONE) ||
			(Window.To.Y	 == INDEX_NONE && Window.To.X		== INDEX_NONE) ||
			(Window.From.Y < y + 1 && Window.From.X < x + 1 && Window.To.Y >= y && Window.To.X >= x)
		) {
			const FVector _location = GetActorLocation();
			const float _x_pos  = ((x * 100 + _location.X) / 100) + 0.01f;
			const float _y_pos  = ((y * 100 + _location.Y) / 100) + 0.01f;
			const float _scaled = (Noise->GetNoise2D(_x_pos, _y_pos) + 1) * (Dimensions.Z / 2);
			const uint8 _value  = FMath::Clamp(FMath::RoundToInt(_scaled), 0, Dimensions.Z - 1);

			const int _current = GetBlockIndex(FVector(x, y, _value));
			
			Surface[_current] |= Block::Surface::Exists | Block::Surface::Up;

			// #if UE_EDITOR
			// 	const uint8 _shade = (static_cast<float>(_value) / Dimensions.Z - 1) * 255;
			//
			// 	DrawDebugBox(
			// 		GetWorld(),
			// 		(FVector(x, y, _value) * 100 + _location) + FVector(-50, -50, 50),
			// 		FVector::OneVector * 50,
			// 		FColor(_shade, 0, _shade, 255),
			// 		true
			// 	);
			// #endif

			bool _filled_in = false;
			int  _offset		= 1;

			while (y > 0 && x > 0 && !_filled_in) {
				const bool _left_neighbor_exists   = (Surface[GetBlockIndex(FVector(x,		  y - 1, _value))] & Block::Surface::Exists) == Block::Surface::Exists;
				const bool _bottom_neighbor_exists = (Surface[GetBlockIndex(FVector(x - 1, y,	   _value))] & Block::Surface::Exists) == Block::Surface::Exists;
				
				if (_left_neighbor_exists && _bottom_neighbor_exists) {
					_filled_in = true;
				
					continue;
				}
			
				if (_value + _offset < Dimensions.Z && _value - _offset > -1) {
					if (!_left_neighbor_exists)
						_filled_in = ProcessNeighbor(
							true,
							FVector2D(x, y),
							_value,
							_offset
						);

					if (!_bottom_neighbor_exists)
						_filled_in = ProcessNeighbor(
							false,
							FVector2D(x, y),
							_value,
							_offset
						);
					
					++_offset;
				}
				else
					_filled_in = true;
			}
		}
	}
}

void AVoxuelChunkBase::GenerateMesh() {
	// Implemented in children
}

int AVoxuelChunkBase::GetBlockIndex(const FVector& position) const {
	return
		 position.Y +
		(position.X *  (Dimensions.Y + 2)) +
		(position.Z * ((Dimensions.X + 2) * (Dimensions.Y + 2)));
}

int AVoxuelChunkBase::GetBlockMeshIndex(const FVector& position) const {
	return
		 (position.Y + 1) +
		((position.X + 1) *  (Dimensions.Y + 2)) +
		 (position.Z			* ((Dimensions.X + 2) * (Dimensions.Y + 2)));
}

bool AVoxuelChunkBase::ProcessNeighbor(
	const bool width,
	const FVector2D& position,
	const uint8 value,
	const uint8 offset
) {
	const uint8 _y = position.Y + (width ? -1 :  0);
	const uint8 _x = position.X + (width ?  0 : -1);

	bool _filled_in = false;
	
	if (Surface[GetBlockIndex(FVector(_x, _y, value + offset))] & Block::Surface::Exists) {
		for (int h = value + 1; h <= value + offset; h++)
			Surface[GetBlockIndex(FVector(_x, _y, h))] |= width ? Block::Surface::Right : Block::Surface::Front;

		_filled_in = true;
	}

	if (Surface[GetBlockIndex(FVector(_x, _y, value - offset))] & Block::Surface::Exists) {
		for (int h = value; h > value - offset; h--)
			Surface[GetBlockIndex(FVector(position.X, position.Y, h))] |= width ? Block::Surface::Left : Block::Surface::Back;

		_filled_in = true;
	}

	return _filled_in;
}
