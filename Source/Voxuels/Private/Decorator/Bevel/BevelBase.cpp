// Fill out your copyright notice in the Description page of Project Settings.

#include "Decorator/Bevel/VoxuelDecoratorBevelBase.h"

void UVoxuelDecoratorBevelBase::Configure() {
  // Implemented in subclasses
}

TArray<FVector> UVoxuelDecoratorBevelBase::GenerateOffsets(const TArray<FVector> points) const {
  return { FVector::ZeroVector };
}

bool UVoxuelDecoratorBevelBase::Apply(const FVector anchor) const {
  return false;
}