# Handoff: New Ages Of War

Last updated: 2026-07-13

## Current state

The repository contains the complete UE 5.8 Phase 1 MVP code slice. `Content/Maps/PrototypeLandscape.umap` is the default game and editor map. `ANAWGameMode` adds the runtime units, structures, resources, cover, enemies, and objective.

## First commands for the next session

```powershell
Set-Location 'G:\My Drive\Codex Coworker\New-Age-Of-War'
./scripts/Run-Validation.ps1
& 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe' .\NewAgeOfWar.uproject
```

In the editor, play `PrototypeLandscape`, build both structure types, possess each unit type, hold fire on enemies, press `K`, wait for command view, and possess another unit.

## Automated verification completed

- `Run-Validation.ps1` passes the live-text audit, UE 5.8 editor build, and authored-map null-RHI boot.
- `NewAgeOfWar Win64 Development` compiles as a standalone game target.
- Runtime boot confirms the authored Landscape, two resource points, expected initial pulse totals, and five controlled enemies.

## Automated-run note

`scripts/Run-Validation.ps1` has passed. It uses one build action and disables accelerated execution because this workstation produced corrupt generated resources under high memory pressure. The script then loads the real map, reaches BeginPlay, and checks explicit startup markers.

## Recommended next scope

Run a hands-on PIE feel pass, replace primitive visuals with Blueprint content children, add firing and death effects, and tune movement, costs, enemy pressure, and camera sensitivity. Keep aircraft, multiplayer, and large-scale optimization in later phases.

## Important implementation notes

- `ANAWPlayerController` owns input and the possession state machine. Do not bind the same actions in units.
- Unit classes are Blueprintable. Use Blueprint children for meshes, weapons, and effects rather than editing C++ defaults for content iteration.
- The enemy controller deliberately uses direct steering so the MVP has no NavMesh setup dependency.
- The map generator lives in the editor-only `NewAgeOfWarEditorTools` module. It is excluded from standalone builds.
- Update this file, `README.md`, and `DEVELOPMENT_LOG.md` in any future change set.
- After verification, commit and push every completed local change to GitHub. Do not leave verified work only in a local checkout.
