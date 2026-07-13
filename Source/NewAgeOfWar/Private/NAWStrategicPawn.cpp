#include "NAWStrategicPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ANAWStrategicPawn::ANAWStrategicPawn()
{
    PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 2200.0f;
    CameraBoom->SetRelativeRotation(FRotator(-58.0f, 35.0f, 0.0f));
    CameraBoom->bDoCollisionTest = false;
    CameraBoom->bUsePawnControlRotation = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;
}

void ANAWStrategicPawn::Pan(const FVector2D& Input)
{
    if (Input.IsNearlyZero())
    {
        return;
    }

    const FRotator ViewRotation = CameraBoom->GetRelativeRotation();
    const FVector Forward = FRotationMatrix(FRotator(0.0f, ViewRotation.Yaw, 0.0f)).GetUnitAxis(EAxis::X);
    const FVector Right = FRotationMatrix(FRotator(0.0f, ViewRotation.Yaw, 0.0f)).GetUnitAxis(EAxis::Y);
    AddActorWorldOffset((Forward * Input.Y + Right * Input.X) * PanSpeed * GetWorld()->GetDeltaSeconds());
}

void ANAWStrategicPawn::Look(const FVector2D& Input)
{
    FRotator ViewRotation = CameraBoom->GetRelativeRotation();
    ViewRotation.Yaw += Input.X;
    ViewRotation.Pitch = FMath::Clamp(ViewRotation.Pitch + Input.Y, -80.0f, -25.0f);
    CameraBoom->SetRelativeRotation(ViewRotation);
}
