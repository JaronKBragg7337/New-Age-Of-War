#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NAWHUD.generated.h"

/** Minimal native HUD that keeps the full Phase 1 loop readable without content assets. */
UCLASS()
class NEWAGEOFWAR_API ANAWHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

private:
    void DrawRTSStatus(float Left, float& Top);
    void DrawFPSStatus(float Left, float& Top);
    void DrawObjective(float Left, float& Top);
    void DrawCrosshair() const;
};
