#include "NAWGameMode.h"

#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Landscape.h"
#include "NAWEnemyUnit.h"
#include "NAWHUD.h"
#include "NAWMatchObjective.h"
#include "NAWPlayerController.h"
#include "NAWResourcePoint.h"
#include "NAWResourceWalletComponent.h"
#include "NAWStrategicPawn.h"
#include "NAWStructures.h"
#include "NAWUnits.h"

ANAWGameMode::ANAWGameMode()
{
    PlayerControllerClass = ANAWPlayerController::StaticClass();
    DefaultPawnClass = ANAWStrategicPawn::StaticClass();
    HUDClass = ANAWHUD::StaticClass();
}

void ANAWGameMode::BeginPlay()
{
    Super::BeginPlay();
    SpawnTestArena();
    GetWorldTimerManager().SetTimerForNextTick(
        this, &ANAWGameMode::ActivateResourceGeneration);
}

void ANAWGameMode::SpawnTestArena()
{
    if (!HasAuthority())
    {
        return;
    }

    UStaticMesh* BlockMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!BlockMesh)
    {
        return;
    }

    auto SpawnBlock = [this, BlockMesh](const FVector& Location, const FVector& Scale)
    {
        AStaticMeshActor* Block = GetWorld()->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
        if (Block)
        {
            UStaticMeshComponent* Mesh = Block->GetStaticMeshComponent();
            Mesh->SetMobility(EComponentMobility::Movable);
            Mesh->SetStaticMesh(BlockMesh);
            Mesh->SetWorldScale3D(Scale);
            Mesh->SetCollisionProfileName(TEXT("BlockAll"));
        }
    };

    // A broad, flat-ish prototype battlefield with lanes and cover. An authored
    // Landscape can replace these primitives without changing gameplay systems.
    bool bHasAuthoredLandscape = false;
    for (TActorIterator<ALandscape> It(GetWorld()); It; ++It)
    {
        bHasAuthoredLandscape = true;
        break;
    }
    if (!bHasAuthoredLandscape)
    {
        SpawnBlock(FVector(0.0f, 0.0f, -50.0f), FVector(180.0f, 180.0f, 1.0f));
    }
    SpawnBlock(FVector(1300.0f, -900.0f, 100.0f), FVector(1.0f, 6.0f, 2.0f));
    SpawnBlock(FVector(2100.0f, 900.0f, 100.0f), FVector(5.0f, 1.0f, 2.0f));
    SpawnBlock(FVector(3000.0f, -300.0f, 100.0f), FVector(2.0f, 2.0f, 2.0f));
    SpawnBlock(FVector(-200.0f, 1900.0f, 75.0f), FVector(7.0f, 1.0f, 1.5f));
    SpawnBlock(FVector(-2400.0f, -1200.0f, 100.0f), FVector(1.0f, 5.0f, 2.0f));
    SpawnBlock(FVector(4300.0f, 1700.0f, 50.0f), FVector(5.0f, 5.0f, 1.0f));

    ANAWBarracks* StartingBase = GetWorld()->SpawnActor<ANAWBarracks>(
        FVector(-850.0f, -550.0f, 0.0f), FRotator::ZeroRotator);
    if (StartingBase)
    {
        StartingBase->FinalizeConstruction();
    }

    GetWorld()->SpawnActor<ANAWInfantry>(FVector(350.0f, 0.0f, 88.0f), FRotator::ZeroRotator);
    GetWorld()->SpawnActor<ANAWInfantry>(FVector(350.0f, 300.0f, 88.0f), FRotator::ZeroRotator);
    GetWorld()->SpawnActor<ANAWReconDrone>(FVector(650.0f, -300.0f, 300.0f), FRotator::ZeroRotator);
    GetWorld()->SpawnActor<ANAWLightArmoredVehicle>(FVector(850.0f, 350.0f, 88.0f), FRotator::ZeroRotator);

    ANAWResourcePoint* SuppliesPoint = GetWorld()->SpawnActor<ANAWResourcePoint>(
        FVector(-1600.0f, 700.0f, 20.0f), FRotator::ZeroRotator);
    if (SuppliesPoint)
    {
        SuppliesPoint->ConfigureResourcePoint(ENAWResourceType::Supplies, 25, 4.0f);
        RuntimeResourcePoints.Add(SuppliesPoint);
    }

    ANAWResourcePoint* FuelPoint = GetWorld()->SpawnActor<ANAWResourcePoint>(
        FVector(-1300.0f, 1200.0f, 20.0f), FRotator::ZeroRotator);
    if (FuelPoint)
    {
        FuelPoint->ConfigureResourcePoint(ENAWResourceType::Fuel, 15, 5.0f);
        RuntimeResourcePoints.Add(FuelPoint);
    }

    const TArray<FVector> EnemyLocations = {
        FVector(5200.0f, -1700.0f, 88.0f),
        FVector(5700.0f, -600.0f, 88.0f),
        FVector(5500.0f, 600.0f, 88.0f),
        FVector(5100.0f, 1600.0f, 88.0f),
        FVector(6500.0f, 0.0f, 88.0f)
    };
    for (const FVector& Location : EnemyLocations)
    {
        GetWorld()->SpawnActor<ANAWEnemyUnit>(Location, FRotator(0.0f, 180.0f, 0.0f));
    }
    GetWorld()->SpawnActor<ANAWMatchObjective>(FVector::ZeroVector, FRotator::ZeroRotator);
    UE_LOG(LogTemp, Display,
        TEXT("Phase 1 arena ready: 4 friendly units, 5 enemies, 2 resource points."));
    UE_LOG(LogTemp, Display, TEXT("Battlefield source: %s"),
        bHasAuthoredLandscape ? TEXT("authored Landscape") : TEXT("runtime fallback"));
}

void ANAWGameMode::ActivateResourceGeneration()
{
    ANAWPlayerController* NAWController = Cast<ANAWPlayerController>(
        UGameplayStatics::GetPlayerController(this, 0));
    UNAWResourceWalletComponent* Wallet = NAWController
        ? NAWController->GetResourceWallet()
        : nullptr;
    if (!Wallet)
    {
        return;
    }

    for (ANAWResourcePoint* ResourcePoint : RuntimeResourcePoints)
    {
        if (IsValid(ResourcePoint))
        {
            ResourcePoint->StartGeneratingForWallet(Wallet);
        }
    }
    int32 ControlledEnemyCount = 0;
    for (TActorIterator<ANAWEnemyUnit> It(GetWorld()); It; ++It)
    {
        ControlledEnemyCount += It->GetController() ? 1 : 0;
    }
    UE_LOG(LogTemp, Display,
        TEXT("Resource generation activated: Supplies=%d Fuel=%d EnemiesControlled=%d"),
        Wallet->GetSupplies(), Wallet->GetFuel(), ControlledEnemyCount);
}
