#include "NAWPossessableUnit.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "NAWPlayerController.h"

ANAWPossessableUnit::ANAWPossessableUnit()
{
    PrimaryActorTick.bCanEverTick = false;
    bUseControllerRotationYaw = true;

    Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
    Capsule->InitCapsuleSize(42.0f, 88.0f);
    Capsule->SetCollisionProfileName(TEXT("Pawn"));
    RootComponent = Capsule;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    VisualMesh->SetupAttachment(Capsule);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(Capsule);
    FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 60.0f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement"));
    Movement->MaxSpeed = 600.0f;
    Movement->Acceleration = 2400.0f;
    Movement->Deceleration = 3000.0f;
}

void ANAWPossessableUnit::BeginPlay()
{
    Super::BeginPlay();
    Health = MaxHealth;
}

float ANAWPossessableUnit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (!bIsAlive || DamageAmount <= 0.0f)
    {
        return 0.0f;
    }

    const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    Health = FMath::Max(0.0f, Health - DamageAmount);
    if (Health <= 0.0f)
    {
        Eliminate();
    }
    return AppliedDamage;
}

void ANAWPossessableUnit::Move(const FVector2D& Input)
{
    if (!bIsAlive)
    {
        return;
    }

    const FRotator ControlRotation = Controller ? Controller->GetControlRotation() : GetActorRotation();
    const FRotationMatrix YawMatrix(FRotator(0.0f, ControlRotation.Yaw, 0.0f));
    AddMovementInput(YawMatrix.GetUnitAxis(EAxis::X), Input.Y);
    AddMovementInput(YawMatrix.GetUnitAxis(EAxis::Y), Input.X);
}

void ANAWPossessableUnit::Look(const FVector2D& Input)
{
    AddControllerYawInput(Input.X);
    AddControllerPitchInput(Input.Y);
}

void ANAWPossessableUnit::FireWeapon()
{
    if (!bIsAlive || !bCanFire || !Controller)
    {
        return;
    }

    bCanFire = false;
    GetWorldTimerManager().SetTimer(FireCooldownTimer, this, &ANAWPossessableUnit::ResetFire, FireCooldown, false);

    FVector ViewLocation;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    FHitResult Hit;
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(UnitWeapon), false, this);
    const FVector End = ViewLocation + ViewRotation.Vector() * WeaponRange;
    if (GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, End, ECC_Visibility, QueryParams) && Hit.GetActor())
    {
        UGameplayStatics::ApplyDamage(Hit.GetActor(), WeaponDamage, Controller, this, UDamageType::StaticClass());
    }
}

void ANAWPossessableUnit::Eliminate()
{
    if (!bIsAlive)
    {
        return;
    }

    bIsAlive = false;
    Health = 0.0f;
    Movement->StopMovementImmediately();
    Movement->SetComponentTickEnabled(false);
    Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetSimulatePhysics(true);
    OnUnitDeath();

    if (ANAWPlayerController* NAWController = Cast<ANAWPlayerController>(Controller))
    {
        NAWController->HandleControlledUnitDeath(this);
    }

    SetLifeSpan(12.0f);
}

void ANAWPossessableUnit::OnUnitDeath_Implementation()
{
    // Blueprint children add death effects here.
}

void ANAWPossessableUnit::ResetFire()
{
    bCanFire = true;
}
