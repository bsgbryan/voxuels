// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetMathLibrary.h"
#include "Chunk/VoxuelChunkGeometry.h"
#include "Block.h"
#include "MatrixTypes.h"

UVoxuelChunkGeometry::UVoxuelChunkGeometry() {}

void UVoxuelChunkGeometry::Add(
	const Block::Face face,
	const FVector& position,
	const FIntVector3& dimensions
) {	
	Vertices.Append(GenerateQuad(face, position, dimensions));

	const FVector _v1 = Vertices[VertexCount];
	const FVector _v2 = Vertices[VertexCount + 2];
	const FVector _v3 = Vertices[VertexCount + 3];
	
	const FVector _normal			= UKismetMathLibrary::Cross_VectorVector(_v1 - _v3, _v2 - _v3);
	const FVector _normalized = UKismetMathLibrary::Normal(_normal, 0.1f);
	
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

	float _depth  = 1;
	float _width  = 1;
	float _offset_depth = 0;
	float _offset_width = 0;

	if (face == Block::Face::Up) {
		_depth = dimensions.X + 1;
		_width = dimensions.Y + 1;
		_offset_depth = static_cast<int>(position.X) % 2;
		_offset_width = static_cast<int>(position.Y) % 2;
	} else if (face == Block::Face::Front || face == Block::Face::Back) {
		_depth = 1;
		_width = dimensions.Y + 1;
		_offset_depth = static_cast<int>(position.Z) % 2;
		_offset_width = static_cast<int>(position.Y) % 2;
	}  else if (face == Block::Face::Left || face == Block::Face::Right) {
		_depth = 1;
		_width = dimensions.X + 1;
		_offset_depth = static_cast<int>(position.Z) % 2;
		_offset_width = static_cast<int>(position.X) % 2;
	}
	
	UVs.Append({
		FVector2D(_offset_depth, _offset_width + _width),
		FVector2D(_offset_depth, _offset_width),
		FVector2D(_offset_depth + _depth, _offset_width),
		FVector2D(_offset_depth + _depth, _offset_width + _width)
	});

	VertexCount += 4;
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
			_vertices[0] = Block::Vertices[Block::Triangles[f][0]] + ((position + FVector(dimensions.X, dimensions.Y, 0)) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][1]] + ((position + FVector(dimensions.X, 0, 0)) * 100);
			_vertices[2] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(dimensions.X, 0, dimensions.Z)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(dimensions.X, dimensions.Y, dimensions.Z)) * 100);

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
			_vertices[0] = Block::Vertices[Block::Triangles[f][0]] + ((position + FVector(dimensions.X, 0, 0)) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][1]] + ((position) * 100);
			_vertices[2] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(0, 0, dimensions.Z)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(dimensions.X, 0, dimensions.Z)) * 100);
				
			break;
		}
		case Block::Face::Right: {
			_vertices[0] = Block::Vertices[Block::Triangles[f][0]] + ((position + FVector(0, dimensions.Y, 0)) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][1]] + ((position + FVector(dimensions.X, dimensions.Y, 0)) * 100);
			_vertices[2] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(dimensions.X, dimensions.Y, dimensions.Z)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(0, dimensions.Y, dimensions.Z)) * 100);
				
			break;
		}
		case Block::Face::Up:
		case Block::Face::Down: {
			_vertices[0] = Block::Vertices[Block::Triangles[f][0]] + ((position + FVector(0, dimensions.Y, 0)) * 100);
			_vertices[1] = Block::Vertices[Block::Triangles[f][1]] + ((position) * 100);
			_vertices[2] = Block::Vertices[Block::Triangles[f][2]] + ((position + FVector(dimensions.X, 0, 0)) * 100);
			_vertices[3] = Block::Vertices[Block::Triangles[f][3]] + ((position + FVector(dimensions.X, dimensions.Y, 0)) * 100);

			break;
		}
	}

	return _vertices;
}
