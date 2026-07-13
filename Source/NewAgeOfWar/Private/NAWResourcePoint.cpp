#include "NAWResourcePoint.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "NAWResourceWalletComponent.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ANAWResourcePoint::ANAWResourcePoint()
{
    PrimaryActorTick.bCanEverTick = false;

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
    SetRootComponent(VisualMesh);
    VisualMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    VisualMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.35f));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(
        TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (CylinderMesh.Succeeded())
    {
        VisualMesh->SetStaticMesh(CylinderMesh.Object);
    }
}

bool ANAWResourcePoint::StartGeneratingForActor(AActor* RecipientActor)
{
    return RecipientActor
        ? StartGeneratingForWallet(RecipientActor->FindComponentByClass<UNAWResourceWalletComponent>())
        : false;
}

bool ANAWResourcePoint::StartGeneratingForWallet(UNAWResourceWalletComponent* Recipient)
{
    StopGenerating();
    if (!IsValid(Recipient) || IsDepleted() || AmountPerPulse <= 0 || PulseInterval <= 0.0f)
    {
        return false;
    }

    RecipientWallet = Recipient;
    if (bGenerateImmediately)
    {
        GenerateOnce();
    }

    if (!IsDepleted() && IsValid(RecipientWallet))
    {
        GetWorldTimerManager().SetTimer(GenerationTimer, this,
            &ANAWResourcePoint::HandleGenerationPulse, PulseInterval, true);
    }
    return true;
}

void ANAWResourcePoint::StopGenerating()
{
    GetWorldTimerManager().ClearTimer(GenerationTimer);
    RecipientWallet = nullptr;
}

int32 ANAWResourcePoint::GenerateOnce()
{
    if (!IsValid(RecipientWallet) || IsDepleted() || AmountPerPulse <= 0)
    {
        return 0;
    }

    const int32 RequestedAmount = RemainingAmount < 0
        ? AmountPerPulse
        : FMath::Min(AmountPerPulse, RemainingAmount);
    const int32 DepositedAmount = RecipientWallet->AddResource(ResourceType, RequestedAmount);
    if (DepositedAmount <= 0)
    {
        return 0;
    }

    if (RemainingAmount > 0)
    {
        RemainingAmount = FMath::Max(0, RemainingAmount - DepositedAmount);
    }

    OnResourceExtracted.Broadcast(ResourceType, DepositedAmount, RemainingAmount);
    if (IsDepleted())
    {
        GetWorldTimerManager().ClearTimer(GenerationTimer);
        OnDepleted.Broadcast();
    }
    return DepositedAmount;
}

bool ANAWResourcePoint::IsGenerating() const
{
    return GetWorldTimerManager().IsTimerActive(GenerationTimer);
}

void ANAWResourcePoint::ConfigureResourcePoint(ENAWResourceType NewType,
    int32 NewAmountPerPulse, float NewPulseInterval, int32 NewRemainingAmount)
{
    StopGenerating();
    ResourceType = NewType;
    AmountPerPulse = FMath::Max(1, NewAmountPerPulse);
    PulseInterval = FMath::Max(0.1f, NewPulseInterval);
    RemainingAmount = FMath::Max(-1, NewRemainingAmount);
}

void ANAWResourcePoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(GenerationTimer);
    Super::EndPlay(EndPlayReason);
}

void ANAWResourcePoint::HandleGenerationPulse()
{
    if (!IsValid(RecipientWallet))
    {
        StopGenerating();
        return;
    }
    GenerateOnce();
}
