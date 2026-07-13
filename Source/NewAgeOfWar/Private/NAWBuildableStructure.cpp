#include "NAWBuildableStructure.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"

ANAWBuildableStructure::ANAWBuildableStructure()
{
    PrimaryActorTick.bCanEverTick = false;
    SetCanBeDamaged(false);

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    StructureCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("StructureCollision"));
    StructureCollision->SetupAttachment(SceneRoot);
    StructureCollision->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(SceneRoot);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ConfigurePlacementBounds(PlacementBoundsExtent, PlacementBoundsOffset);
}

void ANAWBuildableStructure::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
    SetPlacementPreviewState(bIsPlacementPreview);
}

float ANAWBuildableStructure::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (!bConstructionComplete || DamageAmount <= 0.0f)
    {
        return 0.0f;
    }

    const float AppliedDamage = FMath::Min(Health, DamageAmount);
    Health = FMath::Max(0.0f, Health - AppliedDamage);
    Super::TakeDamage(AppliedDamage, DamageEvent, EventInstigator, DamageCauser);
    if (Health <= 0.0f)
    {
        Destroy();
    }
    return AppliedDamage;
}

bool ANAWBuildableStructure::CanPlaceAtTransform(const FTransform& CandidateTransform) const
{
    if (!GetWorld())
    {
        return false;
    }

    FCollisionObjectQueryParams ObjectQuery;
    ObjectQuery.AddObjectTypesToQuery(ECC_WorldStatic);
    ObjectQuery.AddObjectTypesToQuery(ECC_WorldDynamic);
    ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(NAWBuildingPlacement), false, this);
    QueryParams.AddIgnoredActor(this);

    const FVector QueryLocation = CandidateTransform.TransformPosition(PlacementBoundsOffset);
    const FQuat QueryRotation = CandidateTransform.GetRotation();
    const FCollisionShape QueryShape = FCollisionShape::MakeBox(PlacementBoundsExtent);
    return !GetWorld()->OverlapAnyTestByObjectType(
        QueryLocation, QueryRotation, ObjectQuery, QueryShape, QueryParams);
}

void ANAWBuildableStructure::SetPlacementPreviewState(bool bPreview)
{
    bIsPlacementPreview = bPreview;
    SetActorEnableCollision(!bPreview);
    StructureCollision->SetCollisionEnabled(
        bPreview ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);

    if (bPreview && HasActorBegunPlay())
    {
        CacheOriginalMaterials();
    }
    else
    {
        RestoreOriginalMaterials();
        VisualMesh->SetRenderCustomDepth(false);
    }
    OnPlacementPreviewStateChanged(bPreview);
}

void ANAWBuildableStructure::SetPlacementValidity(bool bIsValid)
{
    if (!bIsPlacementPreview)
    {
        return;
    }

    CacheOriginalMaterials();
    UMaterialInterface* PreviewMaterial = bIsValid
        ? ValidPreviewMaterial.Get()
        : InvalidPreviewMaterial.Get();
    if (PreviewMaterial)
    {
        const int32 MaterialCount = VisualMesh->GetNumMaterials();
        for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
        {
            VisualMesh->SetMaterial(MaterialIndex, PreviewMaterial);
        }
    }

    VisualMesh->SetRenderCustomDepth(true);
    VisualMesh->SetCustomDepthStencilValue(bIsValid ? 1 : 2);
    OnPlacementValidityChanged(bIsValid);
}

void ANAWBuildableStructure::FinalizeConstruction()
{
    if (bConstructionComplete)
    {
        return;
    }

    SetPlacementPreviewState(false);
    bConstructionComplete = true;
    SetCanBeDamaged(true);
    Tags.AddUnique(TEXT("FriendlyBase"));
    OnConstructionFinalized();
    OnConstructionCompleted.Broadcast(this);
}

void ANAWBuildableStructure::ConfigurePlacementBounds(const FVector& Extent, const FVector& Offset)
{
    PlacementBoundsExtent = Extent.ComponentMax(FVector(1.0f));
    PlacementBoundsOffset = Offset;
    if (StructureCollision)
    {
        StructureCollision->SetBoxExtent(PlacementBoundsExtent);
        StructureCollision->SetRelativeLocation(PlacementBoundsOffset);
    }
}

void ANAWBuildableStructure::CacheOriginalMaterials()
{
    if (!VisualMesh || OriginalMaterials.Num() > 0)
    {
        return;
    }

    const int32 MaterialCount = VisualMesh->GetNumMaterials();
    OriginalMaterials.Reserve(MaterialCount);
    for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
    {
        OriginalMaterials.Add(VisualMesh->GetMaterial(MaterialIndex));
    }
}

void ANAWBuildableStructure::RestoreOriginalMaterials()
{
    if (!VisualMesh)
    {
        return;
    }

    for (int32 MaterialIndex = 0; MaterialIndex < OriginalMaterials.Num(); ++MaterialIndex)
    {
        VisualMesh->SetMaterial(MaterialIndex, OriginalMaterials[MaterialIndex]);
    }
}
