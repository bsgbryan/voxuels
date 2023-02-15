// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxuelWorld.h"
#include "Chunk/VoxuelChunkBase.h"

AVoxuelWorld::AVoxuelWorld() {
 	PrimaryActorTick.bCanEverTick = false;
}

void AVoxuelWorld::BeginPlay() {
	Super::BeginPlay();

	for (int i = 0, x = DrawDistance; i < DrawDistance * DrawDistance; i++) {
		const int y = i % DrawDistance;

		x = i % DrawDistance == 0 ? x - 1 : x;

		const auto _location = FVector(
			(x * ChunkDimensions.X * 100) + Offset.X * 100,
			(y * ChunkDimensions.Y * 100) + Offset.Y * 100,
			Offset.Z * 100
		);

		const auto _chunk = GetWorld()->SpawnActor<AVoxuelChunkBase>(Chunk, _location, FRotator::ZeroRotator);

		_chunk->Generate(Threaded, ChunkDimensions, Seed, Frequency);
	}
}

