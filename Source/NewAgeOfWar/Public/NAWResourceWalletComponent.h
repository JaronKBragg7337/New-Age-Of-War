#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NAWEconomyTypes.h"
#include "NAWResourceWalletComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNAWResourcesChangedSignature,
    int32, Supplies, int32, Fuel);

/**
 * Stores Supplies and Fuel for one economy owner. Add this component to a
 * player state, controller, pawn, or faction manager Blueprint.
 */
UCLASS(ClassGroup = (NAW), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class NEWAGEOFWAR_API UNAWResourceWalletComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UNAWResourceWalletComponent();

    /** Current amount of one resource. */
    UFUNCTION(BlueprintPure, Category = "NAW|Economy")
    int32 GetResource(ENAWResourceType ResourceType) const;

    UFUNCTION(BlueprintPure, Category = "NAW|Economy")
    int32 GetSupplies() const { return Supplies; }

    UFUNCTION(BlueprintPure, Category = "NAW|Economy")
    int32 GetFuel() const { return Fuel; }

    /** True when this wallet can pay the complete cost. */
    UFUNCTION(BlueprintPure, Category = "NAW|Economy")
    bool CanAfford(const FNAWResourceCost& Cost) const;

    /**
     * Pays a complete cost atomically. No resources are removed when the
     * wallet cannot afford the cost or the cost contains a negative value.
     */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy")
    bool TrySpend(const FNAWResourceCost& Cost);

    /** Adds one resource and returns the amount accepted after capacity limits. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy")
    int32 AddResource(ENAWResourceType ResourceType, int32 Amount);

    /** Adds both resource values and applies configured capacity limits. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy")
    void AddResources(int32 SuppliesToAdd, int32 FuelToAdd);

    /** Replaces both values. This is useful for save loading and test setup. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy")
    void SetResources(int32 NewSupplies, int32 NewFuel);

    /** Broadcast after either stored value changes. */
    UPROPERTY(BlueprintAssignable, Category = "NAW|Economy")
    FNAWResourcesChangedSignature OnResourcesChanged;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Starting Values",
        meta = (ClampMin = "0"))
    int32 InitialSupplies = 500;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Starting Values",
        meta = (ClampMin = "0"))
    int32 InitialFuel = 250;

    /** Zero means that Supplies have no capacity limit. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Capacity",
        meta = (ClampMin = "0"))
    int32 MaxSupplies = 0;

    /** Zero means that Fuel has no capacity limit. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Capacity",
        meta = (ClampMin = "0"))
    int32 MaxFuel = 0;

private:
    UPROPERTY(VisibleInstanceOnly, Category = "NAW|Economy")
    int32 Supplies = 0;

    UPROPERTY(VisibleInstanceOnly, Category = "NAW|Economy")
    int32 Fuel = 0;

    int32 ClampResource(int32 Value, int32 Maximum) const;
    void BroadcastIfChanged(int32 PreviousSupplies, int32 PreviousFuel);
};
