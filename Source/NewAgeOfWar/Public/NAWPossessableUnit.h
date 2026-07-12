#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NAWPossessableUnit.generated.h"

class UCameraComponent;
class UCapsuleComponent;
class UFloatingPawnMovement;
class UStaticMeshComponent;

/**
 * Base class for every friendly unit the commander can enter. Child Blueprints
 * can replace visuals and tune the exposed movement, health, and weapon values.
 */
UCLASS(Blueprintable)
class NEWAGEOFWAR_API ANAWPossessableUnit : public APawn
{
    GENERATED_BODY()

public:
    ANAWPossessableUnit();

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    void Move(const FVector2D& Input);
    void Look(const FVector2D& Input);
    void FireWeapon();
    void Eliminate();

    UFUNCTION(BlueprintPure, Category = "Unit")
    bool IsAlive() const { return bIsAlive; }

    UFUNCTION(BlueprintPure, Category = "Unit")
    bool IsFriendly() const { return bFriendly; }

    UFUNCTION(BlueprintPure, Category = "Unit")
    float GetHealth() const { return Health; }

    UFUNCTION(BlueprintNativeEvent, Category = "Unit")
    void OnUnitDeath();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCapsuleComponent> Capsule;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCameraComponent> FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UFloatingPawnMovement> Movement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
    bool bFriendly = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unit", meta = (ClampMin = "1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Unit")
    float Health = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float WeaponDamage = 25.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float WeaponRange = 5000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    float FireCooldown = 0.16f;

private:
    bool bIsAlive = true;
    bool bCanFire = true;
    FTimerHandle FireCooldownTimer;

    void ResetFire();
};
