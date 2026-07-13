#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NAWBuildingPlacementComponent.generated.h"

class ANAWBuildableStructure;
class UNAWResourceWalletComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNAWBuildingPlacedSignature,
    ANAWBuildableStructure*, Building);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNAWPlacementValidityChangedSignature,
    bool, bIsValid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNAWPlacementCanceledSignature);

/**
 * Blueprint-facing placement workflow. Attach this component to the strategic
 * pawn or player controller, assign a wallet, then call StartPlacement,
 * UpdatePlacementFromRay, and ConfirmPlacement from input logic.
 */
UCLASS(ClassGroup = (NAW), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class NEWAGEOFWAR_API UNAWBuildingPlacementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UNAWBuildingPlacementComponent();

    /** Starts a preview for BuildingClass and cancels any previous preview. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    bool StartPlacement(TSubclassOf<ANAWBuildableStructure> BuildingClass,
        const FTransform& InitialTransform);

    /** Traces a screen-derived world ray and moves the preview to the hit surface. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    bool UpdatePlacementFromRay(const FVector& RayStart, const FVector& RayEnd,
        float DesiredYawDegrees);

    /** Moves the preview to a known hit result, such as Get Hit Result Under Cursor. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    bool UpdatePlacementFromHit(const FHitResult& SurfaceHit, float DesiredYawDegrees);

    /** Moves the preview and performs a vertical ground trace at the desired position. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    bool UpdatePlacementTransform(const FTransform& DesiredTransform);

    /** Adds yaw to the active preview and refreshes placement validation. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    bool RotatePreview(float YawDeltaDegrees);

    /** Pays the build cost and converts a valid preview into a live structure. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    ANAWBuildableStructure* ConfirmPlacement();

    /** Destroys the active preview without charging resources. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    void CancelPlacement();

    /** Overrides automatic wallet discovery for this placement component. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Building|Placement")
    void SetResourceWallet(UNAWResourceWalletComponent* Wallet);

    UFUNCTION(BlueprintPure, Category = "NAW|Building|Placement")
    bool HasActivePlacement() const;

    UFUNCTION(BlueprintPure, Category = "NAW|Building|Placement")
    bool IsCurrentPlacementValid() const { return bCurrentPlacementValid; }

    UFUNCTION(BlueprintPure, Category = "NAW|Building|Placement")
    ANAWBuildableStructure* GetPreviewBuilding() const { return PreviewBuilding.Get(); }

    UPROPERTY(BlueprintAssignable, Category = "NAW|Building|Placement")
    FNAWBuildingPlacedSignature OnBuildingPlaced;

    UPROPERTY(BlueprintAssignable, Category = "NAW|Building|Placement")
    FNAWPlacementValidityChangedSignature OnPlacementValidityChanged;

    UPROPERTY(BlueprintAssignable, Category = "NAW|Building|Placement")
    FNAWPlacementCanceledSignature OnPlacementCanceled;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Building|Placement")
    bool bSnapToGrid = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Building|Placement",
        meta = (ClampMin = "1.0"))
    float GridSize = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Building|Placement",
        meta = (ClampMin = "10.0"))
    float VerticalTraceDistance = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Building|Placement",
        meta = (ClampMin = "0.0", ClampMax = "89.0"))
    float MaximumGroundSlopeDegrees = 25.0f;

private:
    UPROPERTY(Transient)
    TObjectPtr<ANAWBuildableStructure> PreviewBuilding;

    UPROPERTY(Transient)
    TObjectPtr<UNAWResourceWalletComponent> ResourceWallet;

    bool bCurrentPlacementValid = false;
    bool bLastSurfaceValid = false;

    UNAWResourceWalletComponent* ResolveResourceWallet();
    FVector SnapLocation(const FVector& Location) const;
    bool ApplySurfaceTransform(const FVector& Location, const FVector& SurfaceNormal,
        float DesiredYawDegrees);
    bool RefreshPlacementValidity();
    bool CanAffordPreview();
    void SetCurrentValidity(bool bNewValidity);
};
