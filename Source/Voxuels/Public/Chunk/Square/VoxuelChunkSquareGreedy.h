// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk/VoxuelChunkBase.h"
#include "VoxuelChunkSquareGreedy.generated.h"

UCLASS()
class VOXUELS_API AVoxuelChunkSquareGreedy final : public AVoxuelChunkBase {
	GENERATED_BODY()
	
public:
	virtual void GenerateMesh(
		const TObjectPtr<UVoxuelChunkGeometry> geometry,
		TArray<bool> surface,
		const FIntVector size
	) const override;
};

