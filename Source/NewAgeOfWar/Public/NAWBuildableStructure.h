#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NAWEconomyTypes.h"
#include "NAWBuildableStructure.generated.h"

class UBoxComponent;
class UMaterialInterface;
class USceneComponent;
class UStaticMeshComponent;
class ANAWBuildableStructure;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNAWConstructionCompletedSignature,
    ANAWBuildableStructure*, Structure);

/** Base actor for structures that can be previewed and placed by the commander. */
UCLASS(Abstract, BlueprintType, Blueprintable)
class NEWAGEOFWAR_API ANAWBuildableStructure : public AActor
{
    GENERATED_BODY()

public:
    ANAWBuildableStructure();

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION(BlueprintPure, Category = "NAW|Building")
    FNAWResourceCost GetBuildCost() const { return BuildCost; }

    UFUNCTION(BlueprintPure, Category = "NAW|Building")
    FText GetStructureDisplayName() const { return StructureDisplayName; }

    UFUNCTION(BlueprintPure, Category = "NAW|Building")
    bool IsPlacementPreview() const { return bIsPlacementPreview; }

    UFUNCTION(BlueprintPure, Category = "NAW|Building")
    bool IsConstructionComplete() const { return bConstructionComplete; }

    UFUNCTION(BlueprintPure, Category = "NAW|Building")
    float GetHealth() const { return Health; }

    /** Tests the configured placement volume against blocking world objects. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    bool CanPlaceAtTransform(const FTransform& CandidateTransform) const;

    /** Enables preview behavior and disables physical collision while previewing. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    void SetPlacementPreviewState(bool bPreview);

    /** Updates preview feedback. Blueprint materials may override the fallback stencil. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    void SetPlacementValidity(bool bIsValid);

    /** Converts a valid preview into a live structure and fires completion events. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    void FinalizeConstruction();

    UFUNCTION(BlueprintPure, Category = "NAW|Building")
    UStaticMeshComponent* GetVisualMesh() const { return VisualMesh; }

    UPROPERTY(BlueprintAssignable, Category = "NAW|Building")
    FNAWConstructionCompletedSignature OnConstructionCompleted;

protected:
    virtual void BeginPlay() override;

    /** Allows native child constructors to keep bounds and collision synchronized. */
    void ConfigurePlacementBounds(const FVector& Extent, const FVector& Offset);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> StructureCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NAW|Building")
    FText StructureDisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NAW|Building")
    FNAWResourceCost BuildCost;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NAW|Building",
        meta = (ClampMin = "1.0"))
    float MaxHealth = 800.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NAW|Building")
    float Health = 800.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NAW|Building|Placement",
        meta = (ClampMin = "1.0"))
    FVector PlacementBoundsExtent = FVector(100.0f, 100.0f, 100.0f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NAW|Building|Placement")
    FVector PlacementBoundsOffset = FVector(0.0f, 0.0f, 105.0f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NAW|Building|Preview")
    TObjectPtr<UMaterialInterface> ValidPreviewMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "NAW|Building|Preview")
    TObjectPtr<UMaterialInterface> InvalidPreviewMaterial;

    /** Blueprint hook for additional preview effects or UI. */
    UFUNCTION(BlueprintImplementableEvent, Category = "NAW|Building|Preview")
    void OnPlacementPreviewStateChanged(bool bPreview);

    /** Blueprint hook for valid and invalid placement feedback. */
    UFUNCTION(BlueprintImplementableEvent, Category = "NAW|Building|Preview")
    void OnPlacementValidityChanged(bool bIsValid);

    /** Blueprint hook for structure-specific completion behavior. */
    UFUNCTION(BlueprintImplementableEvent, Category = "NAW|Building")
    void OnConstructionFinalized();

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;

    UPROPERTY(VisibleInstanceOnly, Category = "NAW|Building|Placement")
    bool bIsPlacementPreview = false;

    UPROPERTY(VisibleInstanceOnly, Category = "NAW|Building")
    bool bConstructionComplete = false;

    void CacheOriginalMaterials();
    void RestoreOriginalMaterials();
};
