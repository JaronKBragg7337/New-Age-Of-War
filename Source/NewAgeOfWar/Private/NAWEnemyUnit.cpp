#include "NAWEnemyUnit.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "NAWEnemyController.h"
#include "UObject/ConstructorHelpers.h"

ANAWEnemyUnit::ANAWEnemyUnit()
{
    bFriendly = false;
    MaxHealth = 125.0f;
    WeaponDamage = AIAttackDamage;
    WeaponRange = AIAttackRange;
    Movement->MaxSpeed = 470.0f;

    AIControllerClass = ANAWEnemyController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(CubeMesh.Object);
        VisualMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -40.0f));
        VisualMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 1.45f));
        VisualMesh->SetRenderCustomDepth(true);
        VisualMesh->SetCustomDepthStencilValue(1);
    }
}

void ANAWEnemyUnit::AttackTarget(AActor* Target, AController* InstigatingController)
{
    if (!IsAlive() || !IsValid(Target) || Target == this)
    {
        return;
    }

    UGameplayStatics::ApplyDamage(
        Target,
        AIAttackDamage,
        InstigatingController,
        this,
        UDamageType::StaticClass());

    OnEnemyAttack(Target);
}

void ANAWEnemyUnit::OnEnemyAttack_Implementation(AActor* Target)
{
    // Blueprint children add attack feedback here.
}
