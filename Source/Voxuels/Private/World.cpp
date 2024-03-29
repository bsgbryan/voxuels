// Fill out your copyright notice in the Description page of Project Settings.

#include "FastNoiseWrapper.h"
#include "ProceduralMeshComponent.h"
#include "VoxuelsLogger.h"
#include "VoxuelWorld.h"
#include "Decorator/Bevel/VoxuelDecoratorBevelBase.h"
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

	DoRenderChunk(Current.Y, Current.X);
		
	if (Current.Y < DrawDistance.Y && Current.X < DrawDistance.X) {
		++Current.Y;

		if (Current.Y == DrawDistance.Y) {
			Current.Y = 0;

			++Current.X;
		}
	}	
}

void AVoxuelWorld::Clear() {
	for (AVoxuelChunkBase* c : Chunks)
		if (c) GetWorld()->DestroyActor(c);

	Chunks.Empty();

	FlushPersistentDebugLines(GetWorld());

	RenderEnvironmentInitialized = false;
}

void AVoxuelWorld::InitializeRenderEnvironment() {
	Clear();

	Chunks.SetNum(DrawDistance.Y * DrawDistance.X * DrawDistance.Z);

	Current = FIntVector::ZeroValue;

	#if UE_EDITOR
		UDebugConfig::RenderSurfaceBoxes = RenderSurfaceBoxes;
		UDebugConfig::RenderSidePoints	 = RenderSidePoints;
	#endif
	
	RenderEnvironmentInitialized = true;
}

bool AVoxuelWorld::DoRenderChunk(const int y, const int x) {
	if (
		y >= RenderFromChunk.Y												&&
		x >= RenderFromChunk.X												&&
		(RenderToChunk.Y == 0 || y < RenderToChunk.Y) &&
		(RenderToChunk.X == 0 || x < RenderToChunk.X)
	) {
		const auto _location = FVector(
			(x * ChunkDimensions.X * 100) + Offset.X * 100,
			(y * ChunkDimensions.Y * 100) + Offset.Y * 100,
			Offset.Z * 100
		);

		const auto _chunk = GetWorld()->SpawnActor<AVoxuelChunkBase>(Chunk, _location, FRotator::ZeroRotator);

		FRenderExtent _window;

		_window.From = RenderFromBlock;
		_window.To   = RenderToBlock;
		
		_chunk->Threaded	 = Threaded;
		_chunk->Dimensions = ChunkDimensions;
		_chunk->Window		 = _window;
		
		_chunk->Noise->SetupFastNoise(EFastNoise_NoiseType::Perlin, Seed, Frequency);

		_chunk->Mesh->SetMaterial(0, ChunkMaterial);
		
		_chunk->Generate(Bevel->GetDefaultObject<UVoxuelDecoratorBevelBase>());

		Chunks[y + (DrawDistance.Y * x)] = _chunk;

		return true;
	}

	if (y >= RenderToChunk.Y || x >= RenderToChunk.X) {
		UE_LOG(Voxuels, Warning, TEXT("Reached end of RenderToChunk(X:%d, Y:%d), nothing more to render for { y:%d, x:%d }"), RenderToChunk.X, RenderToChunk.Y, y, x)

		return false;
	}

	return false;
}

void AVoxuelWorld::DoRenderAll() {
	bool _y_limit_reached = false;
	bool _x_limit_reached = false;
	
	for (int i = 0, x = -1; i < Chunks.Num(); i++) {
		int y = i % DrawDistance.Y;
		
		if (RenderToChunk.Y > 0 && y >= RenderToChunk.Y) {
			y = RenderToChunk.Y - 1;
			_y_limit_reached = true;
		}
		
		x = i % DrawDistance.Y == 0 ? x + 1 : x;

		if (RenderToChunk.X > 0 && x >= RenderToChunk.X) {
			x = RenderToChunk.X - 1;
			_x_limit_reached = true;
		}

		if (!_y_limit_reached || !_x_limit_reached)
			DoRenderChunk(y, x);
		else
			UE_LOG(Voxuels, Warning, TEXT("Reached end of RenderToChunk(X:%d, Y:%d), nothing else to render"), RenderToChunk.X, RenderToChunk.Y)
	}
}
