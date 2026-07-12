#include "NAWUnits.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "UObject/ConstructorHelpers.h"

ANAWInfantry::ANAWInfantry()
{
    MaxHealth = 100.0f;
    WeaponDamage = 25.0f;
    WeaponRange = 4500.0f;
    Movement->MaxSpeed = 600.0f;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(CubeMesh.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
        VisualMesh->SetRelativeScale3D(FVector(0.7f, 0.7f, 1.4f));
    }
}

ANAWReconDrone::ANAWReconDrone()
{
    MaxHealth = 45.0f;
    WeaponDamage = 8.0f;
    WeaponRange = 3000.0f;
    Movement->MaxSpeed = 1250.0f;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(CubeMesh.Object);
        VisualMesh->SetRelativeScale3D(FVector(1.2f, 1.2f, 0.25f));
    }
}

ANAWLightArmoredVehicle::ANAWLightArmoredVehicle()
{
    MaxHealth = 350.0f;
    WeaponDamage = 18.0f;
    WeaponRange = 6000.0f;
    FireCooldown = 0.09f;
    Movement->MaxSpeed = 900.0f;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(CubeMesh.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
        VisualMesh->SetRelativeScale3D(FVector(2.2f, 1.25f, 0.8f));
    }
}
