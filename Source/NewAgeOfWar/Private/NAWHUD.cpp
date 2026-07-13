#include "NAWHUD.h"

#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "NAWBuildingPlacementComponent.h"
#include "NAWMatchObjective.h"
#include "NAWPlayerController.h"
#include "NAWPossessableUnit.h"
#include "NAWResourceWalletComponent.h"

namespace
{
    constexpr float HUDLineHeight = 24.0f;
    const FLinearColor HeaderColor(0.30f, 0.82f, 1.0f, 1.0f);
    const FLinearColor AccentColor(0.48f, 1.0f, 0.50f, 1.0f);
    const FLinearColor WarningColor(1.0f, 0.45f, 0.25f, 1.0f);
}

void ANAWHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas || !GEngine)
    {
        return;
    }

    float Top = 24.0f;
    DrawText(TEXT("NEW AGES OF WAR  //  PHASE 1 OPERATIONS"), HeaderColor,
        28.0f, Top, GEngine->GetMediumFont(), 1.05f, false);
    Top += 34.0f;

    const ANAWPlayerController* NAWController = Cast<ANAWPlayerController>(PlayerOwner);
    if (NAWController && NAWController->IsInRTSView())
    {
        DrawRTSStatus(28.0f, Top);
    }
    else
    {
        DrawFPSStatus(28.0f, Top);
        DrawCrosshair();
    }
    DrawObjective(28.0f, Top);
}

void ANAWHUD::DrawRTSStatus(float Left, float& Top)
{
    const ANAWPlayerController* NAWController = Cast<ANAWPlayerController>(PlayerOwner);
    if (!NAWController)
    {
        return;
    }

    if (const UNAWResourceWalletComponent* Wallet = NAWController->GetResourceWallet())
    {
        DrawText(FString::Printf(TEXT("SUPPLIES  %d     FUEL  %d"),
            Wallet->GetSupplies(), Wallet->GetFuel()), AccentColor,
            Left, Top, GEngine->GetMediumFont(), 1.0f, false);
        Top += 31.0f;
    }

    DrawText(TEXT("BUILD MENU"), HeaderColor, Left, Top,
        GEngine->GetSmallFont(), 1.0f, false);
    Top += HUDLineHeight;
    DrawText(TEXT("[1] Barracks  300 Supplies / 50 Fuel"), FLinearColor::White,
        Left, Top, GEngine->GetSmallFont(), 1.0f, false);
    Top += HUDLineHeight;
    DrawText(TEXT("[2] Drone Pad  200 Supplies / 150 Fuel"), FLinearColor::White,
        Left, Top, GEngine->GetSmallFont(), 1.0f, false);
    Top += HUDLineHeight;

    const UNAWBuildingPlacementComponent* Placement = NAWController->GetBuildingPlacement();
    if (Placement && Placement->HasActivePlacement())
    {
        const bool bValid = Placement->IsCurrentPlacementValid();
        DrawText(bValid ? TEXT("PLACEMENT READY  |  Left-click to build")
                        : TEXT("PLACEMENT BLOCKED  |  Move the cursor"),
            bValid ? AccentColor : WarningColor, Left, Top,
            GEngine->GetSmallFont(), 1.0f, false);
        Top += HUDLineHeight;
        DrawText(TEXT("[R] Rotate     [Escape] Cancel"), FLinearColor::White,
            Left, Top, GEngine->GetSmallFont(), 1.0f, false);
    }
    else
    {
        DrawText(TEXT("Left-click unit: possess  |  WASD: pan  |  Hold RMB: rotate"),
            FLinearColor(0.75f, 0.80f, 0.86f), Left, Top,
            GEngine->GetSmallFont(), 1.0f, false);
    }
    Top += 34.0f;
}

void ANAWHUD::DrawFPSStatus(float Left, float& Top)
{
    const ANAWPossessableUnit* Unit = PlayerOwner
        ? Cast<ANAWPossessableUnit>(PlayerOwner->GetPawn())
        : nullptr;
    const float Health = Unit ? Unit->GetHealth() : 0.0f;
    DrawText(FString::Printf(TEXT("DIRECT CONTROL  //  HEALTH %.0f"), Health),
        AccentColor, Left, Top, GEngine->GetMediumFont(), 1.0f, false);
    Top += 31.0f;
    DrawText(TEXT("WASD: move  |  Mouse: aim  |  Hold LMB: fire  |  Escape: command view"),
        FLinearColor::White, Left, Top, GEngine->GetSmallFont(), 1.0f, false);
    Top += 38.0f;
}

void ANAWHUD::DrawObjective(float Left, float& Top)
{
    const ANAWMatchObjective* Objective = Cast<ANAWMatchObjective>(
        UGameplayStatics::GetActorOfClass(this, ANAWMatchObjective::StaticClass()));
    if (!Objective)
    {
        return;
    }

    const bool bComplete = Objective->GetObjectiveState() == ENAWMatchObjectiveState::Completed;
    const FString ObjectiveText = bComplete
        ? TEXT("VICTORY  //  ALL ENEMY UNITS DESTROYED")
        : FString::Printf(TEXT("OBJECTIVE  //  DESTROY ALL ENEMIES  //  HOSTILES %d"),
            Objective->GetAliveEnemyCount());
    DrawText(ObjectiveText, bComplete ? AccentColor : HeaderColor,
        Left, Top, GEngine->GetMediumFont(), 1.0f, false);
}

void ANAWHUD::DrawCrosshair() const
{
    if (!Canvas)
    {
        return;
    }
    const float CenterX = Canvas->ClipX * 0.5f;
    const float CenterY = Canvas->ClipY * 0.5f;
    Canvas->K2_DrawLine(FVector2D(CenterX - 8.0f, CenterY),
        FVector2D(CenterX + 8.0f, CenterY), 1.5f, FLinearColor::White);
    Canvas->K2_DrawLine(FVector2D(CenterX, CenterY - 8.0f),
        FVector2D(CenterX, CenterY + 8.0f), 1.5f, FLinearColor::White);
}
