#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NAWMatchObjective.generated.h"

/** Victory rule used by the Phase 1 test match. */
UENUM(BlueprintType)
enum class ENAWMatchObjectiveType : uint8
{
    DestroyAllEnemies UMETA(DisplayName = "Destroy All Enemies"),
    SurviveDuration UMETA(DisplayName = "Survive Duration")
};

/** Runtime state for the objective actor. */
UENUM(BlueprintType)
enum class ENAWMatchObjectiveState : uint8
{
    Inactive,
    Active,
    Completed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FNAWObjectiveStartedSignature,
    ENAWMatchObjectiveType,
    ObjectiveType);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FNAWObjectiveProgressSignature,
    ENAWMatchObjectiveType,
    ObjectiveType,
    float,
    RemainingSeconds,
    int32,
    AliveEnemyCount);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FNAWObjectiveCompletedSignature,
    ENAWMatchObjectiveType,
    ObjectiveType);

/**
 * Standalone match objective actor. It can be placed in a map or spawned by a
 * GameMode and reports state through Blueprint-assignable delegates.
 */
UCLASS(Blueprintable)
class NEWAGEOFWAR_API ANAWMatchObjective : public AActor
{
    GENERATED_BODY()

public:
    ANAWMatchObjective();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "Match Objective")
    void StartObjective();

    UFUNCTION(BlueprintCallable, Category = "Match Objective")
    void StopObjective();

    UFUNCTION(BlueprintCallable, Category = "Match Objective")
    void RestartObjective();

    /** Performs an immediate rule evaluation outside the normal timer cadence. */
    UFUNCTION(BlueprintCallable, Category = "Match Objective")
    void EvaluateObjective();

    UFUNCTION(BlueprintPure, Category = "Match Objective")
    ENAWMatchObjectiveState GetObjectiveState() const { return ObjectiveState; }

    UFUNCTION(BlueprintPure, Category = "Match Objective")
    float GetElapsedSeconds() const;

    UFUNCTION(BlueprintPure, Category = "Match Objective")
    float GetRemainingSeconds() const;

    UFUNCTION(BlueprintPure, Category = "Match Objective")
    int32 GetAliveEnemyCount() const;

    UPROPERTY(BlueprintAssignable, Category = "Match Objective")
    FNAWObjectiveStartedSignature OnObjectiveStarted;

    UPROPERTY(BlueprintAssignable, Category = "Match Objective")
    FNAWObjectiveProgressSignature OnObjectiveProgress;

    UPROPERTY(BlueprintAssignable, Category = "Match Objective")
    FNAWObjectiveCompletedSignature OnObjectiveCompleted;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Objective")
    ENAWMatchObjectiveType ObjectiveType = ENAWMatchObjectiveType::DestroyAllEnemies;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Objective")
    bool bAutoStart = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Objective", meta = (ClampMin = "1.0", Units = "s", EditCondition = "ObjectiveType == ENAWMatchObjectiveType::SurviveDuration", EditConditionHides))
    float SurvivalDurationSeconds = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Objective", meta = (ClampMin = "0.05", Units = "s"))
    float EvaluationInterval = 0.25f;

    /** Prevents an empty level from completing before its enemies are spawned. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Match Objective", meta = (EditCondition = "ObjectiveType == ENAWMatchObjectiveType::DestroyAllEnemies", EditConditionHides))
    bool bRequireEnemyObserved = true;

private:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match Objective", meta = (AllowPrivateAccess = "true"))
    ENAWMatchObjectiveState ObjectiveState = ENAWMatchObjectiveState::Inactive;

    FTimerHandle EvaluationTimer;
    float ObjectiveStartTimeSeconds = 0.0f;
    bool bHasObservedEnemy = false;

    void CompleteObjective();
};
