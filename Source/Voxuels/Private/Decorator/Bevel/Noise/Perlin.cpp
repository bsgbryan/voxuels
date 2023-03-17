// Fill out your copyright notice in the Description page of Project Settings.

#include "FastNoiseWrapper.h"
#include "Decorator/Bevel/Noise/VoxuelDecoratorBevelNoisePerlin.h"

UVoxuelDecoratorBevelNoisePerlin::UVoxuelDecoratorBevelNoisePerlin() {
  Noise = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("Noise"));
}

void UVoxuelDecoratorBevelNoisePerlin::Configure() {
  Noise->SetupFastNoise(EFastNoise_NoiseType::Perlin, Seed, Frequency);
}

TArray<FVector> UVoxuelDecoratorBevelNoisePerlin::GenerateOffsets(
  const TArray<FVector> points
) const {
  TArray<FVector> _out;
  _out.SetNum(points.Num());

  const float _x = points[0].X * 0.01f + 0.01f;
  const float _y = points[FMath::RoundToInt(points.Num() * 0.5f)].Y * 0.01f + 0.01f;
  const float _z = points[points.Num() - 1].Z * 0.01f + 0.01f;

  const float _scale = Noise->GetNoise3D(_x, _y, _z) + 1.0f * 0.125f;

  for (const FVector p : points) {
    const FVector s = p * 0.01f + 0.01f;
    
    const float x = Noise->GetNoise3D(s.X, s.Y, s.Z) + 1.0f * _scale;
    const float y = Noise->GetNoise3D(s.Y, s.Z, s.X) + 1.0f * _scale;
    const float z = Noise->GetNoise3D(s.Z, s.X, s.Y) + 1.0f * _scale;

    _out.Add(FVector(x, y, z)); 
  }
  
  return _out;
}

bool UVoxuelDecoratorBevelNoisePerlin::Apply(const FVector anchor) const {
  return Noise->GetNoise3D(anchor.X, anchor.Y, anchor.Z) + 1 * 0.5f > 0.5f;
}

void UVoxuelDecoratorBevelNoisePerlin::Apply(
  const Block::Face face,
  const TArray<FVector> vertices,
  TArray<FVector>& bevel_vertices
) {
  switch (face) {
    case Block::Face::Up:
      for (uint8 v = 0; v < vertices.Num(); v++) {
        bevel_vertices[v].X =+ vertices[v].X;
        bevel_vertices[v].Y =+ vertices[v].Y;
        bevel_vertices[v].Z =+ vertices[v].Z;
      }
    
      break;
    case Block::Face::Down:
      for (uint8 v = 0; v < vertices.Num(); v++) {
        bevel_vertices[v].X =+ vertices[v].X;
        bevel_vertices[v].Y =+ vertices[v].Y;
        bevel_vertices[v].Z =  vertices[v].Z - bevel_vertices[v].Z;
      }
      
      break;
    case Block::Face::Front:
      for (uint8 v = 0; v < vertices.Num(); v++) {
        bevel_vertices[v].X =+ vertices[v].X;
        bevel_vertices[v].Y =  vertices[v].Y + bevel_vertices[v].Z;
        bevel_vertices[v].Z =  vertices[v].Z - bevel_vertices[v].Z;
      }

      break;
    case Block::Face::Back: break;
    case Block::Face::Left: break;
    case Block::Face::Right: break;
  }
}
