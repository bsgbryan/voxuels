// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "Decorator/Bevel/VoxuelDecoratorBevelBase.h"
#include "VoxuelDecoratorBevelNoisePerlin.generated.h"

class UFastNoiseWrapper;

UCLASS()
class VOXUELS_API UVoxuelDecoratorBevelNoisePerlin final : public UVoxuelDecoratorBevelBase {
  GENERATED_BODY()
  
public:
  UVoxuelDecoratorBevelNoisePerlin();
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
  int Seed = 1138;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
  float Frequency = 0.03f;

  virtual void Configure() override;
  
  virtual TArray<FVector> GenerateOffsets(const TArray<FVector> points) const override;

  virtual bool Apply(const FVector anchor) const override;

private:
  TObjectPtr<UFastNoiseWrapper> Noise;

  static void Apply(
    const Block::Face face,
    const TArray<FVector> vertices,
    TArray<FVector>& bevel_vertices
  );
};
