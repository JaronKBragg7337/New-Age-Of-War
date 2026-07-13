#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NAWPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UNAWBuildingPlacementComponent;
class UNAWResourceWalletComponent;
class ANAWBuildableStructure;
class ANAWPossessableUnit;
class ANAWStrategicPawn;
struct FInputActionValue;

/** Owns the RTS-to-first-person state machine and all Phase 1 input routing. */
UCLASS()
class NEWAGEOFWAR_API ANAWPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ANAWPlayerController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void SetupInputComponent() override;

    /** Called by a possessed unit when its health reaches zero. */
    void HandleControlledUnitDeath(ANAWPossessableUnit* DeadUnit);

    UFUNCTION(BlueprintCallable, Category = "Possession")
    void ReturnToRTSView();

    UFUNCTION(BlueprintPure, Category = "Possession")
    bool IsInRTSView() const;

    UFUNCTION(BlueprintPure, Category = "Economy")
    UNAWResourceWalletComponent* GetResourceWallet() const { return ResourceWallet; }

    UFUNCTION(BlueprintPure, Category = "Building")
    UNAWBuildingPlacementComponent* GetBuildingPlacement() const { return BuildingPlacement; }

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Possession")
    float CameraBlendSeconds = 0.28f;

private:
    UPROPERTY()
    TObjectPtr<ANAWStrategicPawn> StrategicPawn;

    UPROPERTY()
    TObjectPtr<UInputMappingContext> GameplayMappingContext;

    UPROPERTY()
    TObjectPtr<UInputAction> MoveAction;

    UPROPERTY()
    TObjectPtr<UInputAction> LookAction;

    UPROPERTY()
    TObjectPtr<UInputAction> PrimaryAction;

    UPROPERTY()
    TObjectPtr<UInputAction> ReturnToRTSAction;

    UPROPERTY()
    TObjectPtr<UInputAction> DebugKillAction;

    UPROPERTY()
    TObjectPtr<UInputAction> RTSLookAction;

    UPROPERTY()
    TObjectPtr<UInputAction> BuildBarracksAction;

    UPROPERTY()
    TObjectPtr<UInputAction> BuildDronePadAction;

    UPROPERTY()
    TObjectPtr<UInputAction> RotateBuildingAction;

    UPROPERTY(VisibleAnywhere, Category = "Economy")
    TObjectPtr<UNAWResourceWalletComponent> ResourceWallet;

    UPROPERTY(VisibleAnywhere, Category = "Building")
    TObjectPtr<UNAWBuildingPlacementComponent> BuildingPlacement;

    FTimerHandle DeathReturnTimer;
    bool bRTSLookHeld = false;
    float PlacementYawDegrees = 0.0f;

    void TryPossessUnitUnderCursor();
    void PossessUnit(ANAWPossessableUnit* Unit);
    void HandleMove(const FInputActionValue& Value);
    void HandleLook(const FInputActionValue& Value);
    void HandlePrimaryStarted();
    void HandlePrimaryCompleted();
    void HandleRTSLookStarted();
    void HandleRTSLookCompleted();
    void HandleDebugKill();
    void HandleBuildBarracks();
    void HandleBuildDronePad();
    void HandleRotateBuilding();
    void HandleEscape();
    void StartBuildingPlacement(TSubclassOf<ANAWBuildableStructure> BuildingClass);
    void UpdateBuildingPlacement();
    void ConfigureInputMode(bool bRTSMode);
};
