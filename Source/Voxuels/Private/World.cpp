// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxuelsLogger.h"
#include "VoxuelWorld.h"
#include "Chunk/VoxuelChunkBase.h"

AVoxuelWorld::AVoxuelWorld() {
 	PrimaryActorTick.bCanEverTick = false;
}

void AVoxuelWorld::RenderAll() {
	InitializeRenderEnvironment();
	
	DoRenderAll();
}

void AVoxuelWorld::RenderChunk() {
	if (!RenderEnvironmentInitialized)
		InitializeRenderEnvironment();

	
}

void AVoxuelWorld::Clear() {
	for (AVoxuelChunkBase* c : Chunks)
		if (c) GetWorld()->DestroyActor(c);

	Chunks.Empty();

	RenderEnvironmentInitialized = false;
}

void AVoxuelWorld::InitializeRenderEnvironment() {
	Clear();

	Chunks.SetNum(DrawDistance * DrawDistance);

	RenderEnvironmentInitialized = true;
}

void AVoxuelWorld::DoRenderAll() {
	for (int i = 0, x = -1; i < DrawDistance * DrawDistance; i++) {
		const int y = i % DrawDistance;
		
		x = i % DrawDistance == 0 ? x + 1 : x;
		
		if ((RenderToChunk.Y == 0 || y < RenderToChunk.Y) && (RenderToChunk.X == 0 || x < RenderToChunk.X)) {
			const auto _location = FVector(
				(x * ChunkDimensions.X * 100) + Offset.X * 100,
				(y * ChunkDimensions.Y * 100) + Offset.Y * 100,
				Offset.Z * 100
			);

			const auto _chunk = GetWorld()->SpawnActor<AVoxuelChunkBase>(Chunk, _location, FRotator::ZeroRotator);

			_chunk->Generate(Threaded, ChunkDimensions, Seed, Frequency);

			Chunks[y + (DrawDistance * x)] = _chunk;
		}
	}
}

void AVoxuelWorld::DoRenderIteration() {
}
