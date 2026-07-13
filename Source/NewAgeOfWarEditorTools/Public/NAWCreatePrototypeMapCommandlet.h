#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "NAWCreatePrototypeMapCommandlet.generated.h"

/** Generates the checked-in Phase 1 landscape map from deterministic height data. */
UCLASS()
class NEWAGEOFWAREDITORTOOLS_API UNAWCreatePrototypeMapCommandlet : public UCommandlet
{
    GENERATED_BODY()

public:
    UNAWCreatePrototypeMapCommandlet();
    virtual int32 Main(const FString& Params) override;
};
