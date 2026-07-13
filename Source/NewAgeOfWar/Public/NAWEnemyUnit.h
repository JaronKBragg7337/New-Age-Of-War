#pragma once

#include "CoreMinimal.h"
#include "NAWPossessableUnit.h"
#include "NAWEnemyUnit.generated.h"

class AController;

/**
 * Basic hostile combat pawn for the Phase 1 test arena. The paired enemy
 * controller handles direct steering and target selection without a NavMesh.
 */
UCLASS(Blueprintable)
class NEWAGEOFWAR_API ANAWEnemyUnit : public ANAWPossessableUnit
{
    GENERATED_BODY()

public:
    ANAWEnemyUnit();

    UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
    float GetAIAttackDamage() const { return AIAttackDamage; }

    UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
    float GetAIAttackRange() const { return AIAttackRange; }

    UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
    float GetAIAttackCooldown() const { return AIAttackCooldown; }

    UFUNCTION(BlueprintPure, Category = "Enemy|Combat")
    float GetAIAcceptanceRadius() const { return AIAcceptanceRadius; }

    UFUNCTION(BlueprintPure, Category = "Enemy|Targeting")
    FName GetFriendlyBaseTag() const { return FriendlyBaseTag; }

    /** Applies the configured attack damage and invokes the Blueprint feedback hook. */
    UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
    void AttackTarget(AActor* Target, AController* InstigatingController);

    /** Blueprint children can add muzzle flashes, audio, tracers, and animation here. */
    UFUNCTION(BlueprintNativeEvent, Category = "Enemy|Combat")
    void OnEnemyAttack(AActor* Target);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.0"))
    float AIAttackDamage = 12.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "1.0", Units = "cm"))
    float AIAttackRange = 1400.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.05", Units = "s"))
    float AIAttackCooldown = 0.55f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Combat", meta = (ClampMin = "0.0", Units = "cm"))
    float AIAcceptanceRadius = 950.0f;

    /** Any damageable actor with this tag is a valid strategic target. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy|Targeting")
    FName FriendlyBaseTag = TEXT("FriendlyBase");
};
