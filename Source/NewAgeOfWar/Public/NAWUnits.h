#pragma once

#include "CoreMinimal.h"
#include "NAWPossessableUnit.h"
#include "NAWUnits.generated.h"

UCLASS(Blueprintable)
class NEWAGEOFWAR_API ANAWInfantry : public ANAWPossessableUnit
{
    GENERATED_BODY()

public:
    ANAWInfantry();
};

UCLASS(Blueprintable)
class NEWAGEOFWAR_API ANAWReconDrone : public ANAWPossessableUnit
{
    GENERATED_BODY()

public:
    ANAWReconDrone();
};

UCLASS(Blueprintable)
class NEWAGEOFWAR_API ANAWLightArmoredVehicle : public ANAWPossessableUnit
{
    GENERATED_BODY()

public:
    ANAWLightArmoredVehicle();
};
