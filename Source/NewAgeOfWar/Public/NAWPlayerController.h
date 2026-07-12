#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NAWPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
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
    virtual void SetupInputComponent() override;

    /** Called by a possessed unit when its health reaches zero. */
    void HandleControlledUnitDeath(ANAWPossessableUnit* DeadUnit);

    UFUNCTION(BlueprintCallable, Category = "Possession")
    void ReturnToRTSView();

    UFUNCTION(BlueprintPure, Category = "Possession")
    bool IsInRTSView() const;

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

    FTimerHandle DeathReturnTimer;

    void TryPossessUnitUnderCursor();
    void PossessUnit(ANAWPossessableUnit* Unit);
    void HandleMove(const FInputActionValue& Value);
    void HandleLook(const FInputActionValue& Value);
    void HandlePrimaryAction();
    void HandleDebugKill();
    void ConfigureInputMode(bool bRTSMode);
};
