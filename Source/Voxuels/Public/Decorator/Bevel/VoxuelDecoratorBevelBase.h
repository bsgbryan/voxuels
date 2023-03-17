// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "VoxuelDecoratorBevelBase.generated.h"

UCLASS()
class VOXUELS_API UVoxuelDecoratorBevelBase : public UObject {
  GENERATED_BODY()
  
public:
  virtual void Configure();
  
  virtual TArray<FVector> GenerateOffsets(const TArray<FVector> points) const;

  virtual bool Apply(const FVector anchor) const;
};
