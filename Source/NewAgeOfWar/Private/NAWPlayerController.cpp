#include "NAWPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "NAWPossessableUnit.h"
#include "NAWStrategicPawn.h"

ANAWPlayerController::ANAWPlayerController()
{
    bAutoManageActiveCameraTarget = false;
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;

    GameplayMappingContext = CreateDefaultSubobject<UInputMappingContext>(TEXT("GameplayMappingContext"));
    MoveAction = CreateDefaultSubobject<UInputAction>(TEXT("MoveAction"));
    LookAction = CreateDefaultSubobject<UInputAction>(TEXT("LookAction"));
    PrimaryAction = CreateDefaultSubobject<UInputAction>(TEXT("PrimaryAction"));
    ReturnToRTSAction = CreateDefaultSubobject<UInputAction>(TEXT("ReturnToRTSAction"));
    DebugKillAction = CreateDefaultSubobject<UInputAction>(TEXT("DebugKillAction"));

    MoveAction->ValueType = EInputActionValueType::Axis2D;
    LookAction->ValueType = EInputActionValueType::Axis2D;
    PrimaryAction->ValueType = EInputActionValueType::Boolean;
    ReturnToRTSAction->ValueType = EInputActionValueType::Boolean;
    DebugKillAction->ValueType = EInputActionValueType::Boolean;

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

void ANAWPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent);
    EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANAWPlayerController::HandleMove);
    EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANAWPlayerController::HandleLook);
    EnhancedInput->BindAction(PrimaryAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandlePrimaryAction);
    EnhancedInput->BindAction(ReturnToRTSAction, ETriggerEvent::Started, this, &ANAWPlayerController::ReturnToRTSView);
    EnhancedInput->BindAction(DebugKillAction, ETriggerEvent::Started, this, &ANAWPlayerController::HandleDebugKill);
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
    else if (StrategicPawn)
    {
        StrategicPawn->Look(Input);
    }
}

void ANAWPlayerController::HandlePrimaryAction()
{
    if (IsInRTSView())
    {
        TryPossessUnitUnderCursor();
        return;
    }

    if (ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(GetPawn()))
    {
        Unit->FireWeapon();
    }
}

void ANAWPlayerController::HandleDebugKill()
{
    if (ANAWPossessableUnit* Unit = Cast<ANAWPossessableUnit>(GetPawn()))
    {
        Unit->Eliminate();
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
