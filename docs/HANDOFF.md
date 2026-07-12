# Handoff: New Ages Of War

Last updated: 2026-07-12

## Current state

The repository contains a UE 5.8 Phase 1 possession prototype. The test arena is dynamically spawned by `ANAWGameMode` on `/Engine/Maps/Entry`; it does not yet contain a checked-in `.umap` landscape. The editor target and no-contractions audit passed locally on 2026-07-12.

## First commands for the next session

```powershell
Set-Location 'G:\My Drive\Codex Coworker\New-Age-Of-War'
./scripts/Run-Validation.ps1
& 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe' .\NewAgeOfWar.uproject
```

In the editor, build `NewAgeOfWarEditor`, play in editor, click a unit, press `K`, wait for command view, and click another unit.

## Required verification before calling Phase 1 complete

- `Run-Validation.ps1` passes without contractions in `Source` or `Config`.
- The editor target compiles with UE 5.8.
- PIE permits possession of infantry, drone, and vehicle.
- `K` returns from the possessed unit to RTS view after the short blend.
- `Escape` returns voluntarily to RTS view.

## Automated-run note

`scripts/Run-Validation.ps1` has passed. It uses two build actions and disables Unreal Build Accelerator execution because this workstation previously produced a corrupt generated resource under high memory pressure. An unattended standalone editor command was not accepted by the local editor before game startup, so use PIE for runtime verification.

## Next approved scope

Build an authored flat-ish `PrototypeLandscape` level with cover and a PlayerStart. Then introduce Supplies and Fuel as a small isolated economy system. Do not start base construction until resource ownership and spend APIs are tested.

## Important implementation notes

- `ANAWPlayerController` owns input and the possession state machine. Do not bind the same actions in units.
- Unit classes are Blueprintable. Use Blueprint children for meshes, weapons, and effects rather than editing C++ defaults for content iteration.
- Enemy AI, resource gathering, buildings, and objectives are deliberately not present in this slice.
- Update this file, `README.md`, and `DEVELOPMENT_LOG.md` in any future change set.
- After verification, commit and push every completed local change to GitHub. Do not leave verified work only in a local checkout.
