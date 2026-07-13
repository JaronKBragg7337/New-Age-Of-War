#include "NAWPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "NAWBuildingPlacementComponent.h"
#include "NAWBuildableStructure.h"
#include "NAWPossessableUnit.h"
#include "NAWResourceWalletComponent.h"
#include "NAWStrategicPawn.h"
#include "NAWStructures.h"

ANAWPlayerController::ANAWPlayerController()
{
    PrimaryActorTick.bCanEverTick = true;
    bAutoManageActiveCameraTarget = false;
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;

    GameplayMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("GameplayMappingContext"));
    MoveAction = CreateDefaultSubobject<UInputAction>(TEXT("MoveAction"));
    LookAction = CreateDefaultSubobject<UInputAction>(TEXT("LookAction"));
    PrimaryAction = CreateDefaultSubobject<UInputAction>(TEXT("PrimaryAction"));
    ReturnToRTSAction = CreateDefaultSubobject<UInputAction>(TEXT("ReturnToRTSAction"));
    DebugKillAction = CreateDefaultSubobject<UInputAction>(TEXT("DebugKillAction"));
    RTSLookAction = CreateDefaultSubobject<UInputAction>(TEXT("RTSLookAction"));
    BuildBarracksAction = CreateDefaultSubobject<UInputAction>(TEXT("BuildBarracksAction"));
    BuildDronePadAction = CreateDefaultSubobject<UInputAction>(TEXT("BuildDronePadAction"));
    RotateBuildingAction = CreateDefaultSubobject<UInputAction>(TEXT("RotateBuildingAction"));
    ResourceWallet = CreateDefaultSubobject<UNAWResourceWalletComponent>(TEXT("ResourceWallet"));
    BuildingPlacement = CreateDefaultSubobject<UNAWBuildingPlacementComponent>(TEXT("BuildingPlacement"));

    MoveAction->ValueType = EInputActionValueType::Axis2D;
    LookAction->ValueType = EInputActionValueType::Axis2D;
    PrimaryAction->ValueType = EInputActionValueType::Boolean;
    ReturnToRTSAction->ValueType = EInputActionValueType::Boolean;
    DebugKillAction->ValueType = EInputActionValueType::Boolean;
    RTSLookAction->ValueType = EInputActionValueType::Boolean;
    BuildBarracksAction->ValueType = EInputActionValueType::Boolean;
    BuildDronePadAction->ValueType = EInputActionValueType::Boolean;
    RotateBuildingAction->ValueType = EInputActionValueType::Boolean;

    FEnhancedActionKeyMapping& MoveForward = GameplayMappingContext->MapKey(MoveAction, EKeys::W);
    MoveForward.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(GameplayMappingContext, NAME_None, RF_Transient));
    CastChecked<UInputModifierSwizzleAxis>(MoveForward.Modifiers.Last())->Order = EInputAxisSwizzle::YXZ;

    FEnhancedActionKeyMapping& MoveBackward = GameplayMappingContext->MapKey(MoveAction, EKeys::S);
    MoveBackward.Modifiers.Add(NewObject<UInputModifierSwizzleAxis>(GameplayMappingContext, NAME_None, RF_Transient));
    CastChecked<UInputModifierSwizzleAxis>(MoveBackward.Modifiers.Last())->Order = EInputAxisSwizzle::YXZ;
    MoveBackward.Modifiers.Add(NewObject<UInputModifierNegate>(GameplayMappingContext, NAME_None, RF_Transient));

    FEnhancedActionKeyMapping& MoveRight = GameplayMappingContext->MapKey(MoveAction, EKeys::D);
    FEnhancedActionKeyMapping& MoveLeft = GameplayMappingContext->MapKey(MoveAction, EKeys::A);
    MoveLeft.Modifiers.Add(NewObject<UInputModifierNegate>(GameplayMappingContext, NAME_None, RF_Transient));
    GameplayMappingContext->MapKey(LookAction, EKeys::Mouse2D);
    GameplayMappingContext->MapKey(PrimaryAction, EKeys::LeftMouseButton);
    GameplayMappingContext->MapKey(ReturnToRTSAction, EKeys::Escape);
    GameplayMappingContext->MapKey(DebugKillAction, EKeys::K);
    GameplayMappingContext->MapKey(RTSLookAction, EKeys::RightMouseButton);
    GameplayMappingContext->MapKey(BuildBarracksAction, EKeys::One);
    GameplayMappingContext->MapKey(BuildDronePadAction, EKeys::Two);
    GameplayMappingContext->MapKey(RotateBuildingAction, EKeys::R);
}

void ANAWPlayerController::BeginPlay()
{
    Super::BeginPlay();
    StrategicPawn = Cast<ANAWStrategicPawn>(GetPawn());
    if (StrategicPawn)
    {
        StrategicPawn->SetActorLocation(FVector(0.0f, 0.0f, 100.0f));
        SetControlRotation(FRotator(-58.0f, 35.0f, 0.0f));
        SetViewTarget(StrategicPawn);
    }

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            InputSubsystem->AddMappingContext(GameplayMappingContext, 0);
        }
    }
    ConfigureInputMode(true);
}

void ANAWPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (IsInRTSView() && BuildingPlacement && BuildingPlacement->HasActivePlacement())
    {
        UpdateBuildingPlacement();
    }
}

void ANAWPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (!ensureMsgf(EnhancedInput, TEXT("Enhanced Input component is required. Check DefaultInput.ini.")))
    {
        return;
    }
    EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANAWPlayerController::HandleMove);
    EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANAWPlayerController::HandleLook);
    EnhancedInput->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandlePrimaryStarted);
    EnhancedInput->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &ANAWPlayerController::HandlePrimaryCompleted);
    EnhancedInput->BindAction(PrimaryAction, ETriggerEvent::Canceled, this, &ANAWPlayerController::HandlePrimaryCompleted);
    EnhancedInput->BindAction(RTSLookAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandleRTSLookStarted);
    EnhancedInput->BindAction(RTSLookAction, ETriggerEvent::Completed, this, &ANAWPlayerController::HandleRTSLookCompleted);
    EnhancedInput->BindAction(RTSLookAction, ETriggerEvent::Canceled, this, &ANAWPlayerController::HandleRTSLookCompleted);
    EnhancedInput->BindAction(ReturnToRTSAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandleEscape);
    EnhancedInput->BindAction(DebugKillAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandleDebugKill);
    EnhancedInput->BindAction(BuildBarracksAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandleBuildBarracks);
    EnhancedInput->BindAction(BuildDronePadAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandleBuildDronePad);
    EnhancedInput->BindAction(RotateBuildingAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandleRotateBuilding);
}

bool ANAWPlayerController::IsInRTSView() const
{
    return GetPawn() == StrategicPawn;
}

void ANAWPlayerController::TryPossessUnitUnderCursor()
{
    FHitResult Hit;
    if (!GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit))
    {
        return;
    }

    PossessUnit(Cast<ANAWPossessableUnit>(Hit.GetActor()));
}

void ANAWPlayerController::PossessUnit(ANAWPossessableUnit* Unit)
{
    if (!Unit || !Unit->IsFriendly() || !Unit->IsAlive())
    {
        return;
    }

    Possess(Unit);
    Unit->StopFiring();
    SetViewTargetWithBlend(Unit, CameraBlendSeconds, EViewTargetBlendFunction::VTBlend_Cubic);
    ConfigureInputMode(false);
}

void ANAWPlayerController::HandleControlledUnitDeath(ANAWPossessableUnit* DeadUnit)
{
    if (DeadUnit != GetPawn() || !StrategicPawn)
    {
        return;
    }

    SetViewTargetWithBlend(StrategicPawn, 0.65f, EViewTargetBlendFunction::VTBlend_EaseInOut);
    GetWorldTimerManager().SetTimer(DeathReturnTimer, this, &ANAWPlayerController::ReturnToRTSView, 0.65f, false);
}

void ANAWPlayerController::ReturnToRTSView()
{
    if (!StrategicPawn || IsInRTSView())
    {
        return;
    }

    if (ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(GetPawn()))
    {
        Unit->StopFiring();
    }
    Possess(StrategicPawn);
    SetViewTargetWithBlend(StrategicPawn, CameraBlendSeconds, EViewTargetBlendFunction::VTBlend_Cubic);
    ConfigureInputMode(true);
}

void ANAWPlayerController::HandleMove(const FInputActionValue& Value)
{
    const FVector2D Input = Value.Get<FVector2D>();
    if (ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(GetPawn()))
    {
        Unit->Move(Input);
    }
    else if (StrategicPawn)
    {
        StrategicPawn->Pan(Input);
    }
}

void ANAWPlayerController::HandleLook(const FInputActionValue& Value)
{
    const FVector2D Input = Value.Get<FVector2D>();
    if (ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(GetPawn()))
    {
        Unit->Look(Input);
    }
    else if (StrategicPawn && bRTSLookHeld)
    {
        StrategicPawn->Look(Input);
    }
}

void ANAWPlayerController::HandlePrimaryStarted()
{
    if (IsInRTSView())
    {
        if (BuildingPlacement && BuildingPlacement->HasActivePlacement())
        {
            BuildingPlacement->ConfirmPlacement();
            return;
        }
        TryPossessUnitUnderCursor();
        return;
    }

    if (ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(GetPawn()))
    {
        Unit->StartFiring();
    }
}

void ANAWPlayerController::HandlePrimaryCompleted()
{
    if (ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(GetPawn()))
    {
        Unit->StopFiring();
    }
}

void ANAWPlayerController::HandleRTSLookStarted()
{
    bRTSLookHeld = true;
}

void ANAWPlayerController::HandleRTSLookCompleted()
{
    bRTSLookHeld = false;
}

void ANAWPlayerController::HandleDebugKill()
{
    if (ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(GetPawn()))
    {
        Unit->Eliminate();
    }
}

void ANAWPlayerController::HandleBuildBarracks()
{
    StartBuildingPlacement(ANAWBarracks::StaticClass());
}

void ANAWPlayerController::HandleBuildDronePad()
{
    StartBuildingPlacement(ANAWDronePad::StaticClass());
}

void ANAWPlayerController::HandleRotateBuilding()
{
    if (IsInRTSView() && BuildingPlacement && BuildingPlacement->HasActivePlacement())
    {
        PlacementYawDegrees = FMath::Fmod(PlacementYawDegrees + 45.0f, 360.0f);
        BuildingPlacement->RotatePreview(45.0f);
    }
}

void ANAWPlayerController::HandleEscape()
{
    if (BuildingPlacement && BuildingPlacement->HasActivePlacement())
    {
        BuildingPlacement->CancelPlacement();
        return;
    }
    ReturnToRTSView();
}

void ANAWPlayerController::StartBuildingPlacement(TSubclassOf<ANAWBuildableStructure> BuildingClass)
{
    if (!IsInRTSView() || !BuildingPlacement || !BuildingClass)
    {
        return;
    }

    FHitResult SurfaceHit;
    FVector InitialLocation = StrategicPawn ? StrategicPawn->GetActorLocation() : FVector::ZeroVector;
    if (GetHitResultUnderCursorByChannel(
        UEngineTypes::ConvertToTraceType(ECC_Visibility), true, SurfaceHit))
    {
        InitialLocation = SurfaceHit.ImpactPoint;
    }

    PlacementYawDegrees = 0.0f;
    BuildingPlacement->SetResourceWallet(ResourceWallet);
    if (BuildingPlacement->StartPlacement(BuildingClass,
        FTransform(FRotator::ZeroRotator, InitialLocation)))
    {
        UpdateBuildingPlacement();
    }
}

void ANAWPlayerController::UpdateBuildingPlacement()
{
    FHitResult SurfaceHit;
    if (GetHitResultUnderCursorByChannel(
        UEngineTypes::ConvertToTraceType(ECC_Visibility), true, SurfaceHit))
    {
        BuildingPlacement->UpdatePlacementFromHit(SurfaceHit, PlacementYawDegrees);
    }
}

void ANAWPlayerController::ConfigureInputMode(bool bRTSMode)
{
    bShowMouseCursor = bRTSMode;
    if (bRTSMode)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetHideCursorDuringCapture(false);
        SetInputMode(InputMode);
    }
    else
    {
        SetInputMode(FInputModeGameOnly());
    }
}
