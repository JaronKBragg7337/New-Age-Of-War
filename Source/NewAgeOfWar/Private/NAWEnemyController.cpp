#include "NAWEnemyController.h"

#include "EngineUtils.h"
#include "NAWEnemyUnit.h"
#include "NAWPossessableUnit.h"

ANAWEnemyController::ANAWEnemyController()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 0.0f;
}

void ANAWEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    TargetRefreshRemaining = 0.0f;
    AttackCooldownRemaining = 0.0f;
    RefreshTarget();
}

void ANAWEnemyController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    ANAWEnemyUnit* Enemy = GetEnemyUnit();
    if (!Enemy || !Enemy->IsAlive())
    {
        ClearTarget();
        return;
    }

    TargetRefreshRemaining -= DeltaSeconds;
    AttackCooldownRemaining = FMath::Max(0.0f, AttackCooldownRemaining - DeltaSeconds);

    if (TargetRefreshRemaining <= 0.0f || !IsValidTarget(CurrentTarget, *Enemy))
    {
        RefreshTarget();
    }

    if (!CurrentTarget)
    {
        return;
    }

    UpdateFacingAndMovement(DeltaSeconds, *Enemy, *CurrentTarget);

    const float DistanceSquared = FVector::DistSquared(Enemy->GetActorLocation(), CurrentTarget->GetActorLocation());
    const float AttackRangeSquared = FMath::Square(Enemy->GetAIAttackRange());
    if (DistanceSquared <= AttackRangeSquared && AttackCooldownRemaining <= 0.0f &&
        HasClearShot(*CurrentTarget, *Enemy))
    {
        Enemy->AttackTarget(CurrentTarget, this);
        AttackCooldownRemaining = Enemy->GetAIAttackCooldown();
    }
}

void ANAWEnemyController::RefreshTarget()
{
    TargetRefreshRemaining = TargetRefreshInterval;

    if (const ANAWEnemyUnit* Enemy = GetEnemyUnit())
    {
        CurrentTarget = FindClosestTarget(*Enemy);
    }
    else
    {
        CurrentTarget = nullptr;
    }
}

void ANAWEnemyController::ClearTarget()
{
    CurrentTarget = nullptr;
    TargetRefreshRemaining = 0.0f;
}

ANAWEnemyUnit* ANAWEnemyController::GetEnemyUnit() const
{
    return Cast<ANAWEnemyUnit>(GetPawn());
}

AActor* ANAWEnemyController::FindClosestTarget(const ANAWEnemyUnit& Enemy) const
{
    AActor* ClosestTarget = nullptr;
    float ClosestDistanceSquared = TNumericLimits<float>::Max();

    for (TActorIterator<ANAWPossessableUnit> It(GetWorld()); It; ++It)
    {
        ANAWPossessableUnit* Candidate = *It;
        if (!IsValidTarget(Candidate, Enemy))
        {
            continue;
        }

        const float DistanceSquared = FVector::DistSquared(Enemy.GetActorLocation(), Candidate->GetActorLocation());
        if (DistanceSquared < ClosestDistanceSquared)
        {
            ClosestDistanceSquared = DistanceSquared;
            ClosestTarget = Candidate;
        }
    }

    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        AActor* Candidate = *It;
        if (!Candidate->ActorHasTag(Enemy.GetFriendlyBaseTag()) || !IsValidTarget(Candidate, Enemy))
        {
            continue;
        }

        const float DistanceSquared = FVector::DistSquared(Enemy.GetActorLocation(), Candidate->GetActorLocation());
        if (DistanceSquared < ClosestDistanceSquared)
        {
            ClosestDistanceSquared = DistanceSquared;
            ClosestTarget = Candidate;
        }
    }

    return ClosestTarget;
}

bool ANAWEnemyController::IsValidTarget(const AActor* Candidate, const ANAWEnemyUnit& Enemy) const
{
    if (!IsValid(Candidate) || Candidate == &Enemy)
    {
        return false;
    }

    if (const ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(Candidate))
    {
        return Unit->IsFriendly() && Unit->IsAlive();
    }

    return Candidate->ActorHasTag(Enemy.GetFriendlyBaseTag()) && !Candidate->IsActorBeingDestroyed();
}

bool ANAWEnemyController::HasClearShot(const AActor& Target, const ANAWEnemyUnit& Enemy) const
{
    const FVector Start = Enemy.GetActorLocation() + FVector(0.0f, 0.0f, AimHeightOffset);
    const FVector End = Target.GetActorLocation() + FVector(0.0f, 0.0f, AimHeightOffset);

    FHitResult Hit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EnemyWeapon), false, &Enemy);
    if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
    {
        return true;
    }

    return Hit.GetActor() == &Target;
}

void ANAWEnemyController::UpdateFacingAndMovement(
    float DeltaSeconds,
    ANAWEnemyUnit& Enemy,
    const AActor& Target)
{
    FVector ToTarget = Target.GetActorLocation() - Enemy.GetActorLocation();
    ToTarget.Z = 0.0f;

    const float DistanceSquared = ToTarget.SizeSquared();
    if (DistanceSquared <= UE_KINDA_SMALL_NUMBER)
    {
        return;
    }

    const FVector Direction = ToTarget.GetSafeNormal();
    const FRotator DesiredRotation = Direction.Rotation();
    const FRotator NewRotation = FMath::RInterpTo(
        Enemy.GetActorRotation(),
        DesiredRotation,
        DeltaSeconds,
        TurnSpeed);
    Enemy.SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));

    const float StopDistance = FMath::Min(Enemy.GetAIAcceptanceRadius(), Enemy.GetAIAttackRange());
    if (DistanceSquared > FMath::Square(StopDistance))
    {
        Enemy.AddMovementInput(Direction, 1.0f);
    }
}
