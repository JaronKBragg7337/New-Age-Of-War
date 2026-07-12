#include "NAWGameMode.h"

#include "Engine/StaticMeshActor.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "NAWPlayerController.h"
#include "NAWStrategicPawn.h"
#include "NAWUnits.h"
#include "UObject/ConstructorHelpers.h"

ANAWGameMode::ANAWGameMode()
{
    PlayerControllerClass = ANAWPlayerController::StaticClass();
    DefaultPawnClass = ANAWStrategicPawn::StaticClass();
}

void ANAWGameMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnTestArena();
}

void ANAWGameMode::SpawnTestArena()
{
    if (!HasAuthority())
    {
        return;
    }

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!CubeMesh.Succeeded())
    {
        return;
    }

    UStaticMesh* BlockMesh = CubeMesh.Object;
    auto SpawnBlock = [this, BlockMesh](const FVector& Location, const FVector& Scale)
    {
        AStaticMeshActor* Block = GetWorld()->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
        if (Block)
        {
            UStaticMeshComponent* Mesh = Block->GetStaticMeshComponent();
            Mesh->SetStaticMesh(BlockMesh);
            Mesh->SetWorldScale3D(Scale);
        }
    };

    // Flat temporary combat space. The authored Landscape test map replaces it in the next phase.
    SpawnBlock(FVector(0.0f, 0.0f, -75.0f), FVector(45.0f, 45.0f, 0.5f));
    SpawnBlock(FVector(950.0f, -600.0f, 100.0f), FVector(1.0f, 5.0f, 2.0f));
    SpawnBlock(FVector(1350.0f, 450.0f, 100.0f), FVector(4.0f, 1.0f, 2.0f));
    SpawnBlock(FVector(1750.0f, -100.0f, 100.0f), FVector(2.0f, 2.0f, 2.0f));

    GetWorld()->SpawnActor<ANAWInfantry>(FVector(500.0f, 0.0f, 100.0f), FRotator::ZeroRotator);
    GetWorld()->SpawnActor<ANAWInfantry>(FVector(500.0f, 300.0f, 100.0f), FRotator::ZeroRotator);
    GetWorld()->SpawnActor<ANAWReconDrone>(FVector(800.0f, -250.0f, 250.0f), FRotator::ZeroRotator);
    GetWorld()->SpawnActor<ANAWLightArmoredVehicle>(FVector(850.0f, 350.0f, 100.0f), FRotator::ZeroRotator);
}
