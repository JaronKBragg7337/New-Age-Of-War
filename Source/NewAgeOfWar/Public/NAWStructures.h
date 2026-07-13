#pragma once

#include "CoreMinimal.h"
#include "NAWBuildableStructure.h"
#include "NAWStructures.generated.h"

/** Phase 1 infantry production structure. */
UCLASS(BlueprintType, Blueprintable)
class NEWAGEOFWAR_API ANAWBarracks : public ANAWBuildableStructure
{
    GENERATED_BODY()

public:
    ANAWBarracks();
};

/** Phase 1 recon drone production and support structure. */
UCLASS(BlueprintType, Blueprintable)
class NEWAGEOFWAR_API ANAWDronePad : public ANAWBuildableStructure
{
    GENERATED_BODY()

public:
    ANAWDronePad();
};
