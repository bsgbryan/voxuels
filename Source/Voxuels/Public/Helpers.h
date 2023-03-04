#pragma once

#include "Helpers.generated.h"

USTRUCT(BlueprintType)
struct FRenderExtent {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
  FIntVector From = FIntVector::NoneValue;
  
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rendering")
  FIntVector To = FIntVector::NoneValue;
};
