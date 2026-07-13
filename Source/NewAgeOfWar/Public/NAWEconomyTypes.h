#pragma once

#include "CoreMinimal.h"
#include "NAWEconomyTypes.generated.h"

/** Resource categories used by the Phase 1 economy. */
UENUM(BlueprintType)
enum class ENAWResourceType : uint8
{
    Supplies UMETA(DisplayName = "Supplies"),
    Fuel UMETA(DisplayName = "Fuel")
};

/** A construction or production cost expressed in both economy resources. */
USTRUCT(BlueprintType)
struct NEWAGEOFWAR_API FNAWResourceCost
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy", meta = (ClampMin = "0"))
    int32 Supplies = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Economy", meta = (ClampMin = "0"))
    int32 Fuel = 0;

    /** Returns true when both values are safe to use as a cost. */
    bool IsValidCost() const
    {
        return Supplies >= 0 && Fuel >= 0;
    }

    /** Returns true when the cost has no resource requirement. */
    bool IsFree() const
    {
        return Supplies == 0 && Fuel == 0;
    }
};
