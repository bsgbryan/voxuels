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

UCLASS()
class VOXUELS_API UDebugConfig final : public UObject {
  GENERATED_BODY()
  
public:
  static inline bool RenderSurfaceBoxes = false;
  static inline bool RenderSidePoints   = false;
};
