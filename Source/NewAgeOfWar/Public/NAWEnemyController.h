#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "NAWEnemyController.generated.h"

class ANAWEnemyUnit;

/**
 * Minimal hostile controller that searches for friendly targets, advances in
 * a straight line, and attacks when it has range and a clear sight line.
 */
UCLASS(Blueprintable)
class NEWAGEOFWAR_API ANAWEnemyController : public AController
{
    GENERATED_BODY()

public:
    ANAWEnemyController();

    virtual void Tick(float DeltaSeconds) override;
    virtual void OnPossess(APawn* InPawn) override;

    /** Forces an immediate target search. */
    UFUNCTION(BlueprintCallable, Category = "Enemy|Targeting")
    void RefreshTarget();

    UFUNCTION(BlueprintCallable, Category = "Enemy|Targeting")
    void ClearTarget();

    UFUNCTION(BlueprintPure, Category = "Enemy|Targeting")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Enemy|Targeting")
    TObjectPtr<AActor> CurrentTarget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Targeting", meta = (ClampMin = "0.05", Units = "s"))
    float TargetRefreshInterval = 0.4f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Movement", meta = (ClampMin = "0.0"))
    float TurnSpeed = 7.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.0", Units = "cm"))
    float AimHeightOffset = 45.0f;

private:
    float TargetRefreshRemaining = 0.0f;
    float AttackCooldownRemaining = 0.0f;

    ANAWEnemyUnit* GetEnemyUnit() const;
    AActor* FindClosestTarget(const ANAWEnemyUnit& Enemy) const;
    bool IsValidTarget(const AActor* Candidate, const ANAWEnemyUnit& Enemy) const;
    bool HasClearShot(const AActor& Target, const ANAWEnemyUnit& Enemy) const;
    void UpdateFacingAndMovement(float DeltaSeconds, ANAWEnemyUnit& Enemy, const AActor& Target);
};
