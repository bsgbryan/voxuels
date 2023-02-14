// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk/VoxuelChunkBase.h"
#include "Chunk/VoxuelChunkGeometry.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseWrapper.h"

AVoxuelChunkBase::AVoxuelChunkBase() {
 	Noise = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("Noise"));

	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Mesh"));
	Mesh->SetCastShadow(true);

	SetRootComponent(Mesh);
}

void AVoxuelChunkBase::Generate(
	const bool threaded,
	const FIntVector size,
	int seed,
	float frequency
) const {
	const auto _location = GetActorLocation();
	const auto _mesh = Mesh;
	const auto _noise = Noise;
	const auto _geometry = NewObject<UVoxuelChunkGeometry>();

	if (threaded)
		AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [
			&,
			_mesh,
			_noise,
			_location,
			_geometry,
			size, 
			seed,
			frequency
		] {
			const TArray<bool> _surface = GenerateSurface(
				_location,
				_noise,
				size,
				seed,
				frequency
			);

			GenerateMesh(_geometry, _surface, size);

			AsyncTask(ENamedThreads::GameThread, [
				_mesh,
				_geometry
			] {
				_mesh->CreateMeshSection(
					0,
					_geometry->Vertices,
					_geometry->Triangles,
					TArray<FVector>(),
					_geometry->UVs,
					TArray<FColor>(),
					TArray<FProcMeshTangent>(),
					false
				);
			});
		});
	else {
		// UE_LOG(LogTemp, Log, TEXT("Location %s"), *_location.ToString())
		const TArray<bool> _surface = GenerateSurface(
			_location,
			Noise,
			size,
			seed,
			frequency
		);

		GenerateMesh(_geometry, _surface, size);

		Mesh->CreateMeshSection(
			0,
			_geometry->Vertices,
			_geometry->Triangles,
			TArray<FVector>(),
			_geometry->UVs,
			TArray<FColor>(),
			TArray<FProcMeshTangent>(),
			false
		);
	}
}

TArray<bool> AVoxuelChunkBase::GenerateSurface(
	const FVector location,
	const TObjectPtr<UFastNoiseWrapper> noise,
	const FIntVector size,
	const int seed,
	const float frequency
) {
	noise->SetupFastNoise(EFastNoise_NoiseType::Perlin, seed, frequency);

	const int _width  = size.Y + 2;
	const int _depth  = size.X + 2;
	const int _height = size.Z;
	
	TArray<bool> _surface;
	_surface.SetNum(_width * _depth * _height);

	for (int i = 0, x = _depth; i < _width * _depth; i++) {
		const int y = i % _width;

		x = i % _width == 0 ? x - 1 : x;

		const float _x_pos  = (((x - 1) * 100 + location.X) / 100) + 0.01f;
		const float _y_pos  = (((y - 1) * 100 + location.Y) / 100) + 0.01f;
		const float _scaled = (noise->GetNoise2D(_x_pos, _y_pos) + 1) * _height / 2;
		const int   _value  = FMath::Clamp(FMath::RoundToInt(_scaled), 0, _height);

		_surface[GetBlockIndex(size, FVector(x, y, _value))] = true;

		bool _filled_in    = false;
		int  _offset	   = 1;
		bool _y_filled_in  = false;
		bool _x_filled_in  = false;
		bool _xy_filled_in = false;

		while (!_filled_in) {
			if (_value + _offset < size.Z && _value - _offset > -1) {
				if (x > 0 && y > 0 && !_surface[GetBlockIndex(size, FVector(x - 1, y - 1, _value))]) {
					const bool _xy_up   = _surface[GetBlockIndex(size, FVector(x - 1, y - 1, _value + _offset))];
					const bool _xy_down = _surface[GetBlockIndex(size, FVector(x - 1, y - 1, _value - _offset))];
				
					if (!_xy_filled_in && _xy_up) {
						_xy_filled_in = true;
						
						for (int h = _value + 1; h < _value + _offset; h++)
							_surface[GetBlockIndex(size, FVector(x - 1, y - 1, h))] = true;
					}

					if (!_xy_filled_in && _xy_down) {
						_xy_filled_in = true;
						
						for (int h = _value - 1; h > _value - _offset; h--)
							_surface[GetBlockIndex(size, FVector(x, y, h))] = true;
					}
				}
				else
					_xy_filled_in = true;

				if (y > 0 && !_surface[GetBlockIndex(size, FVector(x, y - 1, _value))]) {
					const bool _y_up   = _surface[GetBlockIndex(size, FVector(x, y - 1, _value + _offset))];
					const bool _y_down = _surface[GetBlockIndex(size, FVector(x, y - 1, _value - _offset))];
				
					if (!_y_filled_in && _y_up) {
						_y_filled_in = true;
						
						for (int h = _value + 1; h < _value + _offset; h++)
							_surface[GetBlockIndex(size, FVector(x, y - 1, h))] = true;
					}

					if (!_y_filled_in && _y_down) {
						_y_filled_in = true;
						
						for (int h = _value - 1; h > _value - _offset; h--)
							_surface[GetBlockIndex(size, FVector(x, y, h))] = true;
					}
				}
				else
					_y_filled_in = true;

				if (x > 0 && !_surface[GetBlockIndex(size, FVector(x - 1, y, _value))]) {
					const bool _x_up   = _surface[GetBlockIndex(size, FVector(x - 1, y, _value + _offset))];
					const bool _x_down = _surface[GetBlockIndex(size, FVector(x - 1, y, _value - _offset))];
				
					if (!_x_filled_in && _x_up) {
						_x_filled_in = true;
						
						for (int h = _value + 1; h < _value + _offset; h++)
							_surface[GetBlockIndex(size, FVector(x - 1, y, h))] = true;
					}

					if (!_x_filled_in && _x_down) {
						_x_filled_in = true;
						
						for (int h = _value - 1; h > _value - _offset; h--)
							_surface[GetBlockIndex(size, FVector(x, y, h))] = true;
					}
				}
				else
					_x_filled_in = true;
				

				_filled_in = _xy_filled_in && _y_filled_in && _x_filled_in;
				++_offset;
			}
			else
				_filled_in = true;
		}
	}

	return _surface;
}

void AVoxuelChunkBase::GenerateMesh(
	TObjectPtr<UVoxuelChunkGeometry> geometry,
	TArray<bool> surface,
	FIntVector size
) const {
	// Implemented in children
}

int AVoxuelChunkBase::GetBlockIndex(
	const FIntVector size,
	const FVector position
) {
	return
		 static_cast<int>(position.X) +
		(static_cast<int>(position.Y) *  (size.X + 2)) +
		(static_cast<int>(position.Z) * ((size.X + 2) * (size.Y + 2)));
}

int AVoxuelChunkBase::GetBlockMeshIndex(
	const FIntVector size,
	const FVector position
) {
	return
		 (static_cast<int>(position.X) + 1) +
		((static_cast<int>(position.Y) + 1) * (size.X + 2)) +
		(static_cast<int>(position.Z) * ((size.X + 2) * (size.Y + 2)));
}
