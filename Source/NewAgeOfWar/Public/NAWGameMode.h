#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NAWGameMode.generated.h"

/**
 * Starts a self-contained test arena on the Engine Entry map. Replace this
 * runtime arena with authored landscape maps when environment work begins.
 */
UCLASS()
class NEWAGEOFWAR_API ANAWGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ANAWGameMode();

protected:
    virtual void BeginPlay() override;

private:
    void SpawnTestArena();
};
