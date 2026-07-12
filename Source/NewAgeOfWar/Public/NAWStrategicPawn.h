#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "NAWStrategicPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;

/** Top-down command camera used whenever the player is not possessing a unit. */
UCLASS(Blueprintable)
class NEWAGEOFWAR_API ANAWStrategicPawn : public APawn
{
    GENERATED_BODY()

public:
    ANAWStrategicPawn();

    void Pan(const FVector2D& Input);
    void Look(const FVector2D& Input);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USpringArmComponent> CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RTS Camera")
    float PanSpeed = 1800.0f;
};
