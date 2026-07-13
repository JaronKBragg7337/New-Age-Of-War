#include "NAWCreatePrototypeMapCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/FileManager.h"
#include "Landscape.h"
#include "LandscapeLayerInfoObject.h"
#include "Misc/PackageName.h"
#include "NAWGameMode.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"

UNAWCreatePrototypeMapCommandlet::UNAWCreatePrototypeMapCommandlet()
{
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
    ShowErrorCount = true;
}

int32 UNAWCreatePrototypeMapCommandlet::Main(const FString& Params)
{
    constexpr int32 ComponentCount = 4;
    constexpr int32 SectionsPerComponent = 1;
    constexpr int32 QuadsPerSection = 63;
    constexpr int32 QuadsPerComponent = SectionsPerComponent * QuadsPerSection;
    constexpr int32 VertexCount = ComponentCount * QuadsPerComponent + 1;
    const FString MapPackageName = TEXT("/Game/Maps/PrototypeLandscape");

    UPackage* Package = CreatePackage(*MapPackageName);
    if (!Package)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not create the prototype map package."));
        return 1;
    }

    UWorld* World = UWorld::CreateWorld(
        EWorldType::Editor, false, FName(TEXT("PrototypeLandscape")), Package);
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not create the prototype map world."));
        return 1;
    }

    World->SetFlags(RF_Public | RF_Standalone);

    const FVector LandscapeScale(100.0f, 100.0f, 100.0f);
    const float HalfExtent = (VertexCount - 1) * LandscapeScale.X * 0.5f;
    ALandscape* Landscape = World->SpawnActor<ALandscape>(
        FVector(-HalfExtent, -HalfExtent, 0.0f), FRotator::ZeroRotator);
    if (!Landscape)
    {
        UE_LOG(LogTemp, Error, TEXT("Could not spawn the prototype landscape."));
        World->DestroyWorld(false);
        return 1;
    }
    Landscape->SetActorScale3D(LandscapeScale);

    TArray<uint16> Heights;
    Heights.SetNumUninitialized(VertexCount * VertexCount);
    for (int32 Y = 0; Y < VertexCount; ++Y)
    {
        for (int32 X = 0; X < VertexCount; ++X)
        {
            const float NormalizedX = static_cast<float>(X) / (VertexCount - 1);
            const float NormalizedY = static_cast<float>(Y) / (VertexCount - 1);
            const float EdgeShape = FMath::Sin(NormalizedX * PI * 2.0f) * 90.0f
                + FMath::Cos(NormalizedY * PI * 2.0f) * 65.0f;
            const float CenterDistance = FVector2D(
                NormalizedX - 0.5f, NormalizedY - 0.5f).Size();
            const float CenterFlatten = FMath::Clamp((CenterDistance - 0.20f) / 0.30f, 0.0f, 1.0f);
            Heights[Y * VertexCount + X] = static_cast<uint16>(
                FMath::Clamp(32768.0f + EdgeShape * CenterFlatten, 0.0f, 65535.0f));
        }
    }

    TMap<FGuid, TArray<uint16>> HeightData;
    HeightData.Add(FGuid(), MoveTemp(Heights));
    TMap<FGuid, TArray<FLandscapeImportLayerInfo>> MaterialLayerData;
    MaterialLayerData.Add(FGuid(), TArray<FLandscapeImportLayerInfo>());
    Landscape->Import(FGuid::NewGuid(), 0, 0, VertexCount - 1, VertexCount - 1,
        SectionsPerComponent, QuadsPerSection, HeightData, TEXT(""),
        MaterialLayerData, ELandscapeImportAlphamapType::Additive,
        TArrayView<const FLandscapeLayer>());
    Landscape->SetActorLabel(TEXT("Prototype Landscape"));

    APlayerStart* PlayerStart = World->SpawnActor<APlayerStart>(
        FVector::ZeroVector + FVector(0.0f, 0.0f, 250.0f), FRotator::ZeroRotator);
    if (PlayerStart)
    {
        PlayerStart->SetActorLabel(TEXT("RTS Commander Start"));
    }

    ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(
        FVector(0.0f, 0.0f, 3000.0f), FRotator(-48.0f, -35.0f, 0.0f));
    ASkyLight* Sky = World->SpawnActor<ASkyLight>(
        FVector(0.0f, 0.0f, 2500.0f), FRotator::ZeroRotator);
    if (Sun)
    {
        Sun->SetActorLabel(TEXT("Tactical Sun"));
    }
    if (Sky)
    {
        Sky->SetActorLabel(TEXT("Tactical Sky"));
    }

    if (AWorldSettings* WorldSettings = World->GetWorldSettings())
    {
        WorldSettings->DefaultGameMode = ANAWGameMode::StaticClass();
    }

    World->UpdateWorldComponents(true, false);
    Package->MarkPackageDirty();
    FAssetRegistryModule::AssetCreated(World);

    const FString Filename = FPackageName::LongPackageNameToFilename(
        MapPackageName, FPackageName::GetMapPackageExtension());
    IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true);
    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    SaveArgs.SaveFlags = SAVE_NoError;
    const bool bSaved = UPackage::SavePackage(Package, World, *Filename, SaveArgs);
    UE_LOG(LogTemp, Display, TEXT("Prototype landscape map save result: %s"),
        bSaved ? TEXT("success") : TEXT("failure"));
    World->DestroyWorld(false);
    return bSaved ? 0 : 1;
}
