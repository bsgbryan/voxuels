// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetMathLibrary.h"
#include "Chunk/VoxuelChunkGeometry.h"
#include "Block.h"
#include "MatrixTypes.h"
#include "VoxuelsLogger.h"
#include "Decorator/Bevel/VoxuelDecoratorBevelBase.h"

UVoxuelChunkGeometry::UVoxuelChunkGeometry() {}

void UVoxuelChunkGeometry::Add(
	const Block::Face face,
	const FVector& position,
	const UVoxuelDecoratorBevelBase* bevel,
	const FIntVector3& extent,
	const FIntVector3& dimensions
) {
	const TArray<FVector> _vertices = GenerateQuad(face, position, dimensions);
	
	Vertices.Append(_vertices);

	// if (bevel->Apply(position)) {
	// 	const TArray<FVector> _bevel_vertices = bevel->GenerateOffsets(_vertices);
	//
	// 	Apply(face, _vertices, _bevel_vertices);
	// }
	// else {
		const FVector _v1 = Vertices[VertexCount];
		const FVector _v2 = Vertices[VertexCount + 2];
		const FVector _v3 = Vertices[VertexCount + 3];
		
		const FVector _normal			= UKismetMathLibrary::Cross_VectorVector(_v1 - _v3, _v2 - _v3);
		const FVector _normalized = UKismetMathLibrary::Normal(_normal, 0.1f);

		const float _r = FMath::Clamp((abs(_normalized.Y) * 0.5f) + (abs(_normalized.X) * 0.5f), 0.0f, 0.5f) * 2.0f;
		const float _g = FMath::Clamp((abs(_normalized.X) * 0.5f) + (abs(_normalized.Z) * 0.5f), 0.0f, 0.5f) * 2.0f;
		const float _b = FMath::Clamp((abs(_normalized.Z) * 0.5f) + (abs(_normalized.Y) * 0.5f), 0.0f, 0.5f) * 2.0f;

		const float _intensity = (position.Z / Dimensions.Z) * 255;
		
		const FColor _color = FColor(
			_r * _intensity,
			_g * _intensity,
			_b * _intensity,
			1
		);
		
		Normals.Append({
			_normalized,
			_normalized,
			_normalized,
			_normalized
		});
		
		Triangles.Append({
			VertexCount + 3,
			VertexCount + 2,
			VertexCount,
			VertexCount + 2,
			VertexCount + 1,
			VertexCount
		});

		Colors.Append({
			_color,
			_color,
			_color,
			_color
		});

		const FVector _scaled = position * 1.0f;

		TArray<FVector2D> uvs;
		uvs.SetNum(4);
		
		if (face == Block::Face::Up) {
			const float X = static_cast<float>(extent.X);
			const float Y = static_cast<float>(extent.Y);

			const float _depth = (dimensions.X + 1) / X;
			const float _width = (dimensions.Y + 1) / Y;

			const float _offset_depth = _scaled.X / X;
			const float _offset_width = _scaled.Y / Y;

			UVs.Append({
				FVector2D(_offset_depth * X, (_offset_width + _width) * Y),
				FVector2D(_offset_depth * X, _offset_width * Y),
				FVector2D((_offset_depth + _depth) * X, _offset_width * Y),
				FVector2D((_offset_depth + _depth) * X, (_offset_width + _width) * Y),
				// For reference: These are the unit-coordinate equivalents of the above
				// FVector2D(0, 1),
				// FVector2D(0, 0),
				// FVector2D(1, 0),
				// FVector2D(1, 1),
			});
		} else if (face == Block::Face::Front || face == Block::Face::Back) {
			const float Z = static_cast<float>(extent.Z);
			const float Y = static_cast<float>(extent.Y);
			
			const float _depth = 1 / Z;
			const float _width = (dimensions.Y + 1) / Y;

			const float _offset_depth = _scaled.Z / Z;
			const float _offset_width = _scaled.Y / Y;

			UVs.Append({
				FVector2D((_offset_depth + _depth) * Z, _offset_width * Y),
				FVector2D((_offset_depth + _depth) * Z, (_offset_width + _width) * Y),
				FVector2D(_offset_depth * Z, (_offset_width + _width) * Y),
				FVector2D(_offset_depth * Z, _offset_width * Y),
				// For reference: These are the unit-coordinate equivalents of the above
				// FVector2D(1, 0),
				// FVector2D(1, 1),
				// FVector2D(0, 1),
				// FVector2D(0, 0),
			});
		} else if (face == Block::Face::Left || face == Block::Face::Right) {
			const float Z = static_cast<float>(extent.Z);
			const float X = static_cast<float>(extent.X);
			
			const float _depth = 1 / Z;
			const float _width = (dimensions.X + 1) / X;

			const float _offset_depth = _scaled.X / X;
			const float _offset_width = _scaled.Z / Z;
			
			UVs.Append({
				FVector2D(_offset_depth * Z, (_offset_width + _width) * X),
				FVector2D(_offset_depth * Z, _offset_width * X),
				FVector2D((_offset_depth + _depth) * Z, _offset_width * X),
				FVector2D((_offset_depth + _depth) * Z, (_offset_width + _width) * X),
				// For reference: These are the unit-coordinate equivalents of the above
				// FVector2D(0, 1),
				// FVector2D(0, 0),
				// FVector2D(1, 0),
				// FVector2D(1, 1),
			});
		}

		VertexCount += 4;
	// }
}

TArray<FVector> UVoxuelChunkGeometry::GenerateQuad(
	const Block::Face face,
	const FVector& position,
	const FIntVector3& dimensions
) {
	TArray<FVector> _vertices;
	_vertices.SetNum(4);
	const int f = static_cast<int>(face);

	switch (face) {
		case Block::Face::Front: {
			_vertices[2] = Block::Vertices[Block::Triangles[f][0]] + ((position + FVector(dimensions.X, dimensions.Y, 0)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][1]] + ((position + FVector(dimensions.X, 0, 0)) * 100);
			_vertices[0] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(dimensions.X, 0, dimensions.Z)) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(dimensions.X, dimensions.Y, dimensions.Z)) * 100);

			break;		
		}
		case Block::Face::Back: {
			_vertices[0] = Block::Vertices[Block::Triangles[f][0]] + ((position) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][1]] + ((position + FVector(0, dimensions.Y, 0)) * 100);
			_vertices[2] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(0, dimensions.Y, dimensions.Z)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(0, 0, dimensions.Z)) * 100);

			break;		
		}
		case Block::Face::Left: {
			_vertices[2] = Block::Vertices[Block::Triangles[f][0]] + ((position + FVector(dimensions.X, 0, 0)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][1]] + ((position) * 100);
			_vertices[0] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(0, 0, dimensions.Z)) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(dimensions.X, 0, dimensions.Z)) * 100);
				
			break;
		}
		case Block::Face::Right: {
			_vertices[2] = Block::Vertices[Block::Triangles[f][0]] + ((position + FVector(0, dimensions.Y, 0)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][1]] + ((position + FVector(dimensions.X, dimensions.Y, 0)) * 100);
			_vertices[0] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(dimensions.X, dimensions.Y, dimensions.Z)) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(0, dimensions.Y, dimensions.Z)) * 100);
				
			break;
		}
		case Block::Face::Up: {
			_vertices[0] = Block::Vertices[Block::Triangles[f][0]] + ((position + FVector(0, dimensions.Y, 0)) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][1]] + ((position) * 100);
			_vertices[2] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(dimensions.X, 0, 0)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(dimensions.X, dimensions.Y, 0)) * 100);

			break;
		}
	}

	return _vertices;
}
