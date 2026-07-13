#include "NAWMatchObjective.h"

#include "EngineUtils.h"
#include "NAWPossessableUnit.h"
#include "TimerManager.h"

ANAWMatchObjective::ANAWMatchObjective()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ANAWMatchObjective::BeginPlay()
{
    Super::BeginPlay();

    if (bAutoStart)
    {
        StartObjective();
    }
}

void ANAWMatchObjective::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(EvaluationTimer);
    Super::EndPlay(EndPlayReason);
}

void ANAWMatchObjective::StartObjective()
{
    if (ObjectiveState == ENAWMatchObjectiveState::Active)
    {
        return;
    }

    ObjectiveState = ENAWMatchObjectiveState::Active;
    ObjectiveStartTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    bHasObservedEnemy = false;
    OnObjectiveStarted.Broadcast(ObjectiveType);

    const float SafeInterval = FMath::Max(0.05f, EvaluationInterval);
    GetWorldTimerManager().SetTimer(
        EvaluationTimer,
        this,
        &ANAWMatchObjective::EvaluateObjective,
        SafeInterval,
        true);
    EvaluateObjective();
}

void ANAWMatchObjective::StopObjective()
{
    GetWorldTimerManager().ClearTimer(EvaluationTimer);
    if (ObjectiveState == ENAWMatchObjectiveState::Active)
    {
        ObjectiveState = ENAWMatchObjectiveState::Inactive;
    }
}

void ANAWMatchObjective::RestartObjective()
{
    GetWorldTimerManager().ClearTimer(EvaluationTimer);
    ObjectiveState = ENAWMatchObjectiveState::Inactive;
    StartObjective();
}

void ANAWMatchObjective::EvaluateObjective()
{
    if (ObjectiveState != ENAWMatchObjectiveState::Active)
    {
        return;
    }

    const int32 AliveEnemyCount = GetAliveEnemyCount();
    bHasObservedEnemy |= AliveEnemyCount > 0;
    OnObjectiveProgress.Broadcast(ObjectiveType, GetRemainingSeconds(), AliveEnemyCount);

    if (ObjectiveType == ENAWMatchObjectiveType::DestroyAllEnemies)
    {
        if (AliveEnemyCount == 0 && (!bRequireEnemyObserved || bHasObservedEnemy))
        {
            CompleteObjective();
        }
        return;
    }

    if (GetElapsedSeconds() >= SurvivalDurationSeconds)
    {
        CompleteObjective();
    }
}

float ANAWMatchObjective::GetElapsedSeconds() const
{
    if (ObjectiveState == ENAWMatchObjectiveState::Inactive || !GetWorld())
    {
        return 0.0f;
    }

    return FMath::Max(0.0f, GetWorld()->GetTimeSeconds() - ObjectiveStartTimeSeconds);
}

float ANAWMatchObjective::GetRemainingSeconds() const
{
    if (ObjectiveType != ENAWMatchObjectiveType::SurviveDuration)
    {
        return 0.0f;
    }

    return FMath::Max(0.0f, SurvivalDurationSeconds - GetElapsedSeconds());
}

int32 ANAWMatchObjective::GetAliveEnemyCount() const
{
    if (!GetWorld())
    {
        return 0;
    }

    int32 AliveEnemyCount = 0;
    for (TActorIterator<ANAWPossessableUnit> It(GetWorld()); It; ++It)
    {
        const ANAWPossessableUnit* Unit = *It;
        if (!Unit->IsFriendly() && Unit->IsAlive())
        {
            ++AliveEnemyCount;
        }
    }
    return AliveEnemyCount;
}

void ANAWMatchObjective::CompleteObjective()
{
    if (ObjectiveState != ENAWMatchObjectiveState::Active)
    {
        return;
    }

    GetWorldTimerManager().ClearTimer(EvaluationTimer);
    ObjectiveState = ENAWMatchObjectiveState::Completed;
    OnObjectiveCompleted.Broadcast(ObjectiveType);
}
