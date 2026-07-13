#include "NAWResourceWalletComponent.h"

UNAWResourceWalletComponent::UNAWResourceWalletComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UNAWResourceWalletComponent::BeginPlay()
{
    Super::BeginPlay();
    Supplies = ClampResource(InitialSupplies, MaxSupplies);
    Fuel = ClampResource(InitialFuel, MaxFuel);
    OnResourcesChanged.Broadcast(Supplies, Fuel);
}

int32 UNAWResourceWalletComponent::GetResource(ENAWResourceType ResourceType) const
{
    return ResourceType == ENAWResourceType::Supplies ? Supplies : Fuel;
}

bool UNAWResourceWalletComponent::CanAfford(const FNAWResourceCost& Cost) const
{
    return Cost.IsValidCost() && Supplies >= Cost.Supplies && Fuel >= Cost.Fuel;
}

bool UNAWResourceWalletComponent::TrySpend(const FNAWResourceCost& Cost)
{
    if (!CanAfford(Cost))
    {
        return false;
    }

    const int32 PreviousSupplies = Supplies;
    const int32 PreviousFuel = Fuel;
    Supplies -= Cost.Supplies;
    Fuel -= Cost.Fuel;
    BroadcastIfChanged(PreviousSupplies, PreviousFuel);
    return true;
}

int32 UNAWResourceWalletComponent::AddResource(ENAWResourceType ResourceType, int32 Amount)
{
    if (Amount <= 0)
    {
        return 0;
    }

    const int32 PreviousSupplies = Supplies;
    const int32 PreviousFuel = Fuel;
    int32 AcceptedAmount = 0;

    if (ResourceType == ENAWResourceType::Supplies)
    {
        Supplies = ClampResource(Supplies + Amount, MaxSupplies);
        AcceptedAmount = Supplies - PreviousSupplies;
    }
    else
    {
        Fuel = ClampResource(Fuel + Amount, MaxFuel);
        AcceptedAmount = Fuel - PreviousFuel;
    }

    BroadcastIfChanged(PreviousSupplies, PreviousFuel);
    return AcceptedAmount;
}

void UNAWResourceWalletComponent::AddResources(int32 SuppliesToAdd, int32 FuelToAdd)
{
    const int32 PreviousSupplies = Supplies;
    const int32 PreviousFuel = Fuel;
    Supplies = ClampResource(Supplies + FMath::Max(0, SuppliesToAdd), MaxSupplies);
    Fuel = ClampResource(Fuel + FMath::Max(0, FuelToAdd), MaxFuel);
    BroadcastIfChanged(PreviousSupplies, PreviousFuel);
}

void UNAWResourceWalletComponent::SetResources(int32 NewSupplies, int32 NewFuel)
{
    const int32 PreviousSupplies = Supplies;
    const int32 PreviousFuel = Fuel;
    Supplies = ClampResource(NewSupplies, MaxSupplies);
    Fuel = ClampResource(NewFuel, MaxFuel);
    BroadcastIfChanged(PreviousSupplies, PreviousFuel);
}

int32 UNAWResourceWalletComponent::ClampResource(int32 Value, int32 Maximum) const
{
    const int32 NonNegativeValue = FMath::Max(0, Value);
    return Maximum > 0 ? FMath::Min(NonNegativeValue, Maximum) : NonNegativeValue;
}

void UNAWResourceWalletComponent::BroadcastIfChanged(int32 PreviousSupplies, int32 PreviousFuel)
{
    if (PreviousSupplies != Supplies || PreviousFuel != Fuel)
    {
        OnResourcesChanged.Broadcast(Supplies, Fuel);
    }
}
