#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NAWEconomyTypes.h"
#include "NAWResourcePoint.generated.h"

class UNAWResourceWalletComponent;
class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FNAWResourceExtractedSignature,
    ENAWResourceType, ResourceType, int32, Amount, int32, RemainingAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNAWResourcePointDepletedSignature);

/**
 * A map resource point that deposits resources into an assigned wallet on a
 * timer. Blueprint logic decides when gathering starts and which wallet owns
 * the output.
 */
UCLASS(BlueprintType, Blueprintable)
class NEWAGEOFWAR_API ANAWResourcePoint : public AActor
{
    GENERATED_BODY()

public:
    ANAWResourcePoint();

    /** Finds a wallet on RecipientActor and starts timed generation. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy|Resource Point")
    bool StartGeneratingForActor(AActor* RecipientActor);

    /** Starts timed generation for an explicit wallet. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy|Resource Point")
    bool StartGeneratingForWallet(UNAWResourceWalletComponent* Recipient);

    /** Stops generation without changing the remaining deposit. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy|Resource Point")
    void StopGenerating();

    /** Performs one generation pulse and returns the amount deposited. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy|Resource Point")
    int32 GenerateOnce();

    UFUNCTION(BlueprintPure, Category = "NAW|Economy|Resource Point")
    bool IsGenerating() const;

    UFUNCTION(BlueprintPure, Category = "NAW|Economy|Resource Point")
    bool IsDepleted() const { return RemainingAmount == 0; }

    UFUNCTION(BlueprintPure, Category = "NAW|Economy|Resource Point")
    int32 GetRemainingAmount() const { return RemainingAmount; }

    UFUNCTION(BlueprintPure, Category = "NAW|Economy|Resource Point")
    ENAWResourceType GetResourceType() const { return ResourceType; }

    /** Sets the runtime deposit values used by the native prototype arena. */
    UFUNCTION(BlueprintCallable, Category = "NAW|Economy|Resource Point")
    void ConfigureResourcePoint(ENAWResourceType NewType, int32 NewAmountPerPulse,
        float NewPulseInterval, int32 NewRemainingAmount = -1);

    UPROPERTY(BlueprintAssignable, Category = "NAW|Economy|Resource Point")
    FNAWResourceExtractedSignature OnResourceExtracted;

    UPROPERTY(BlueprintAssignable, Category = "NAW|Economy|Resource Point")
    FNAWResourcePointDepletedSignature OnDepleted;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Resource Point")
    ENAWResourceType ResourceType = ENAWResourceType::Supplies;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Resource Point",
        meta = (ClampMin = "1"))
    int32 AmountPerPulse = 25;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Resource Point",
        meta = (ClampMin = "0.1"))
    float PulseInterval = 5.0f;

    /** Negative one creates an infinite point. Zero marks a depleted point. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Resource Point",
        meta = (ClampMin = "-1"))
    int32 RemainingAmount = -1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NAW|Economy|Resource Point")
    bool bGenerateImmediately = true;

private:
    UPROPERTY(Transient)
    TObjectPtr<UNAWResourceWalletComponent> RecipientWallet;

    FTimerHandle GenerationTimer;

    void HandleGenerationPulse();
};
