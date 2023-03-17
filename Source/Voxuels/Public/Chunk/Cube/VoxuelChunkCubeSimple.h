// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Chunk/VoxuelChunkBase.h"
#include "VoxuelChunkCubeSimple.generated.h"

UCLASS()
class VOXUELS_API AVoxuelChunkSquareSimple final : public AVoxuelChunkBase {
	GENERATED_BODY()
	
public:
	virtual void GenerateMesh(UVoxuelDecoratorBevelBase* bevel) override;
};
