﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk/VoxuelChunkGeometry.h"
#include "Block.h"

UVoxuelChunkGeometry::UVoxuelChunkGeometry() {}

void UVoxuelChunkGeometry::Add(
	const Block::Face face,
	const FVector position,
	const FIntVector3 dimensions
) {	
	Vertices.Append(GenerateQuad(face, position, dimensions));
	
	UVs.Append({
		FVector2D(1,1),
		FVector2D(1,0),
		FVector2D(0,0),
		FVector2D(0,1)
	});
	
	Triangles.Append({
		VertexCount + 3,
		VertexCount + 2,
		VertexCount,
		VertexCount + 2,
		VertexCount + 1,
		VertexCount
	});

	VertexCount += 4;
}

TArray<FVector> UVoxuelChunkGeometry::GenerateQuad(
	const Block::Face face,
	const FVector position,
	const FIntVector3 dimensions
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