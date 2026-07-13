#include "NAWStructures.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ANAWBarracks::ANAWBarracks()
{
    StructureDisplayName = NSLOCTEXT("NAWBuildings", "Barracks", "Barracks");
    BuildCost.Supplies = 300;
    BuildCost.Fuel = 50;
    ConfigurePlacementBounds(FVector(200.0f, 150.0f, 100.0f), FVector(0.0f, 0.0f, 105.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
        TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(CubeMesh.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
        VisualMesh->SetRelativeScale3D(FVector(4.0f, 3.0f, 2.0f));
    }
}

ANAWDronePad::ANAWDronePad()
{
    StructureDisplayName = NSLOCTEXT("NAWBuildings", "DronePad", "Drone Pad");
    BuildCost.Supplies = 200;
    BuildCost.Fuel = 150;
    ConfigurePlacementBounds(FVector(175.0f, 175.0f, 20.0f), FVector(0.0f, 0.0f, 25.0f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
        TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(CubeMesh.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 20.0f));
        VisualMesh->SetRelativeScale3D(FVector(3.5f, 3.5f, 0.4f));
    }
}
