#include "NAWBuildingPlacementComponent.h"

#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "NAWBuildableStructure.h"
#include "NAWResourceWalletComponent.h"

UNAWBuildingPlacementComponent::UNAWBuildingPlacementComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

bool UNAWBuildingPlacementComponent::HasActivePlacement() const
{
    return IsValid(PreviewBuilding.Get());
}

bool UNAWBuildingPlacementComponent::StartPlacement(
    TSubclassOf<ANAWBuildableStructure> BuildingClass, const FTransform& InitialTransform)
{
    CancelPlacement();
    if (!GetWorld() || !BuildingClass)
    {
        return false;
    }

    PreviewBuilding = GetWorld()->SpawnActorDeferred<ANAWBuildableStructure>(
        BuildingClass, InitialTransform, GetOwner(), nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
    if (!IsValid(PreviewBuilding))
    {
        return false;
    }

    PreviewBuilding->SetPlacementPreviewState(true);
    UGameplayStatics::FinishSpawningActor(PreviewBuilding, InitialTransform);
    PreviewBuilding->SetPlacementPreviewState(true);
    UpdatePlacementTransform(InitialTransform);
    return true;
}

bool UNAWBuildingPlacementComponent::UpdatePlacementFromRay(
    const FVector& RayStart, const FVector& RayEnd, float DesiredYawDegrees)
{
    if (!IsValid(PreviewBuilding) || !GetWorld())
    {
        return false;
    }

    FHitResult SurfaceHit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(NAWPlacementRay), true, GetOwner());
    QueryParams.AddIgnoredActor(PreviewBuilding);
    if (GetWorld()->LineTraceSingleByChannel(
        SurfaceHit, RayStart, RayEnd, ECC_Visibility, QueryParams))
    {
        return UpdatePlacementFromHit(SurfaceHit, DesiredYawDegrees);
    }

    bLastSurfaceValid = false;
    PreviewBuilding->SetActorLocation(RayEnd);
    return RefreshPlacementValidity();
}

bool UNAWBuildingPlacementComponent::UpdatePlacementFromHit(
    const FHitResult& SurfaceHit, float DesiredYawDegrees)
{
    if (!IsValid(PreviewBuilding) || !SurfaceHit.bBlockingHit)
    {
        return false;
    }

    return ApplySurfaceTransform(
        SurfaceHit.ImpactPoint, SurfaceHit.ImpactNormal, DesiredYawDegrees);
}

bool UNAWBuildingPlacementComponent::UpdatePlacementTransform(const FTransform& DesiredTransform)
{
    if (!IsValid(PreviewBuilding) || !GetWorld())
    {
        return false;
    }

    const FVector DesiredLocation = SnapLocation(DesiredTransform.GetLocation());
    const FVector TraceStart = DesiredLocation + FVector::UpVector * VerticalTraceDistance;
    const FVector TraceEnd = DesiredLocation - FVector::UpVector * VerticalTraceDistance;

    FHitResult GroundHit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(NAWPlacementGround), true, GetOwner());
    QueryParams.AddIgnoredActor(PreviewBuilding);
    if (GetWorld()->LineTraceSingleByChannel(
        GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
    {
        return ApplySurfaceTransform(GroundHit.ImpactPoint, GroundHit.ImpactNormal,
            DesiredTransform.Rotator().Yaw);
    }

    bLastSurfaceValid = false;
    PreviewBuilding->SetActorTransform(DesiredTransform);
    return RefreshPlacementValidity();
}

bool UNAWBuildingPlacementComponent::RotatePreview(float YawDeltaDegrees)
{
    if (!IsValid(PreviewBuilding))
    {
        return false;
    }

    FRotator Rotation = PreviewBuilding->GetActorRotation();
    Rotation.Yaw += YawDeltaDegrees;
    PreviewBuilding->SetActorRotation(FRotator(0.0f, Rotation.Yaw, 0.0f));
    return RefreshPlacementValidity();
}

ANAWBuildableStructure* UNAWBuildingPlacementComponent::ConfirmPlacement()
{
    if (!IsValid(PreviewBuilding) || !RefreshPlacementValidity())
    {
        return nullptr;
    }

    const FNAWResourceCost Cost = PreviewBuilding->GetBuildCost();
    UNAWResourceWalletComponent* Wallet = ResolveResourceWallet();
    if (!Cost.IsFree() && (!Wallet || !Wallet->TrySpend(Cost)))
    {
        RefreshPlacementValidity();
        return nullptr;
    }

    ANAWBuildableStructure* PlacedBuilding = PreviewBuilding;
    PreviewBuilding = nullptr;
    bCurrentPlacementValid = false;
    PlacedBuilding->FinalizeConstruction();
    OnBuildingPlaced.Broadcast(PlacedBuilding);
    return PlacedBuilding;
}

void UNAWBuildingPlacementComponent::CancelPlacement()
{
    const bool bHadPlacement = IsValid(PreviewBuilding);
    if (bHadPlacement)
    {
        PreviewBuilding->Destroy();
    }
    PreviewBuilding = nullptr;
    bCurrentPlacementValid = false;
    bLastSurfaceValid = false;
    if (bHadPlacement)
    {
        OnPlacementCanceled.Broadcast();
    }
}

void UNAWBuildingPlacementComponent::SetResourceWallet(UNAWResourceWalletComponent* Wallet)
{
    ResourceWallet = Wallet;
    if (IsValid(PreviewBuilding))
    {
        RefreshPlacementValidity();
    }
}

void UNAWBuildingPlacementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CancelPlacement();
    Super::EndPlay(EndPlayReason);
}

UNAWResourceWalletComponent* UNAWBuildingPlacementComponent::ResolveResourceWallet()
{
    if (IsValid(ResourceWallet))
    {
        return ResourceWallet;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return nullptr;
    }

    ResourceWallet = Owner->FindComponentByClass<UNAWResourceWalletComponent>();
    if (IsValid(ResourceWallet))
    {
        return ResourceWallet;
    }

    if (const AController* Controller = Cast<AController>(Owner))
    {
        if (APawn* Pawn = Controller->GetPawn())
        {
            ResourceWallet = Pawn->FindComponentByClass<UNAWResourceWalletComponent>();
        }
    }
    else if (const APawn* Pawn = Cast<APawn>(Owner))
    {
        if (AController* PawnController = Pawn->GetController())
        {
            ResourceWallet = PawnController->FindComponentByClass<UNAWResourceWalletComponent>();
        }
    }
    return ResourceWallet;
}

FVector UNAWBuildingPlacementComponent::SnapLocation(const FVector& Location) const
{
    if (!bSnapToGrid || GridSize <= 0.0f)
    {
        return Location;
    }

    return FVector(
        FMath::GridSnap(Location.X, GridSize),
        FMath::GridSnap(Location.Y, GridSize),
        Location.Z);
}

bool UNAWBuildingPlacementComponent::ApplySurfaceTransform(
    const FVector& Location, const FVector& SurfaceNormal, float DesiredYawDegrees)
{
    if (!IsValid(PreviewBuilding))
    {
        return false;
    }

    const FVector SnappedLocation = SnapLocation(Location);
    const float MinimumUpDot = FMath::Cos(FMath::DegreesToRadians(MaximumGroundSlopeDegrees));
    bLastSurfaceValid = FVector::DotProduct(SurfaceNormal.GetSafeNormal(), FVector::UpVector)
        >= MinimumUpDot;
    PreviewBuilding->SetActorLocationAndRotation(
        SnappedLocation, FRotator(0.0f, DesiredYawDegrees, 0.0f));
    return RefreshPlacementValidity();
}

bool UNAWBuildingPlacementComponent::RefreshPlacementValidity()
{
    const bool bNewValidity = IsValid(PreviewBuilding)
        && bLastSurfaceValid
        && PreviewBuilding->CanPlaceAtTransform(PreviewBuilding->GetActorTransform())
        && CanAffordPreview();
    SetCurrentValidity(bNewValidity);
    return bNewValidity;
}

bool UNAWBuildingPlacementComponent::CanAffordPreview()
{
    if (!IsValid(PreviewBuilding))
    {
        return false;
    }

    const FNAWResourceCost Cost = PreviewBuilding->GetBuildCost();
    if (Cost.IsFree())
    {
        return true;
    }

    UNAWResourceWalletComponent* Wallet = ResolveResourceWallet();
    return Wallet && Wallet->CanAfford(Cost);
}

void UNAWBuildingPlacementComponent::SetCurrentValidity(bool bNewValidity)
{
    const bool bChanged = bCurrentPlacementValid != bNewValidity;
    bCurrentPlacementValid = bNewValidity;
    if (IsValid(PreviewBuilding))
    {
        PreviewBuilding->SetPlacementValidity(bCurrentPlacementValid);
    }
    if (bChanged)
    {
        OnPlacementValidityChanged.Broadcast(bCurrentPlacementValid);
    }
}
