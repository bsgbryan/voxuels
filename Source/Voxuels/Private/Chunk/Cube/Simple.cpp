// Fill out your copyright notice in the Description page of Project Settings.

#include "Chunk/Cube/VoxuelChunkCubeSimple.h"
#include "Block.h"
#include "Chunk/VoxuelChunkGeometry.h"

void AVoxuelChunkSquareSimple::GenerateMesh(UVoxuelDecoratorBevelBase* bevel) {
	for (int i = 0, x = -1, z = -1; i < Dimensions.X * Dimensions.Y * Dimensions.Z; i++) {
		const int y = i % Dimensions.Y;

		if (i % (Dimensions.X * Dimensions.Y) == 0)
			x = 0;
		else
			x = i % Dimensions.Y == 0 ? x + 1 : x;
		
		z = i % (Dimensions.X * Dimensions.Y) == 0 ? z + 1 : z;

		const FVector _position = FVector(x, y, z);
		const uint8   _current  = Surface[GetBlockMeshIndex(_position)];

		if ((_current & Block::Surface::Front) == Block::Surface::Front)
			Geometry->Add(Block::Face::Front, _position, bevel, Dimensions);
		
		if ((_current & Block::Surface::Back) == Block::Surface::Back)
			Geometry->Add(Block::Face::Back, _position, bevel, Dimensions);
		
		if ((_current & Block::Surface::Left) == Block::Surface::Left)
			Geometry->Add(Block::Face::Left, _position, bevel, Dimensions);
		
		if ((_current & Block::Surface::Right) == Block::Surface::Right)
			Geometry->Add(Block::Face::Right, _position, bevel, Dimensions);

		if ((_current & Block::Surface::Up) == Block::Surface::Up)
			Geometry->Add(Block::Face::Up, _position, bevel, Dimensions);

		if ((_current & Block::Surface::Down) == Block::Surface::Down)
			Geometry->Add(Block::Face::Down, _position, bevel, Dimensions);
	}
}
