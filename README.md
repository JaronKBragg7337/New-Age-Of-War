# New Ages Of War

New Ages Of War is a near-future combined-arms game that combines real-time strategy command with direct first-person control. The foundational Phase 1 prototype is complete: command from a top-down view, click any friendly unit to enter it, and return automatically to command view when that unit dies.

## Current playable slice

- A runtime test arena with a flat combat ground, cover blocks, and four friendly test units.
- Infantry, recon drone, and light armored vehicle classes with temporary primitive visuals, distinct health, speed, and weapons.
- Enhanced Input controls created in code and shared between RTS and first-person modes.
- Smooth camera blend on possession and a short death return transition.
- Health, hit-scan weapon damage, and a debug self-elimination path for possession testing.

The arena is intentionally created at runtime from the Engine Entry map. It proves the possession loop before landscape and production art are added.

## Run it

1. Open [NewAgeOfWar.uproject](NewAgeOfWar.uproject) with Unreal Engine 5.8.
2. Select a C++ development environment if Unreal prompts for one, then build the `NewAgeOfWarEditor` target.
3. Press Play in Editor. The strategic view opens over the test arena.

### Controls

| Mode | Controls |
| --- | --- |
| RTS command view | `WASD` pan, mouse look, left-click a friendly unit to possess it |
| Possessed unit | `WASD` move, mouse look, left-click fire, `Escape` return to RTS |
| Possession test | While possessed, press `K` to eliminate the unit. After a 0.65-second transition, click another surviving unit. |

## Architecture

The key classes are intentionally small and Blueprint-ready. See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for ownership and extension points.

| Class | Responsibility |
| --- | --- |
| `ANAWPlayerController` | Owns the RTS and first-person state machine, Enhanced Input, selection, and camera transitions. |
| `ANAWStrategicPawn` | Top-down command camera and panning. |
| `ANAWPossessableUnit` | Friendly unit base: FPS camera, movement, health, hitscan weapon, and death signal. |
| `ANAWInfantry`, `ANAWReconDrone`, `ANAWLightArmoredVehicle` | Tuned Phase 1 unit variants that can become Blueprint parents. |
| `ANAWGameMode` | Creates the temporary test arena and initial units. |

## Development rules

- Use UE 5.8 and Enhanced Input.
- Keep gameplay iteration Blueprint-first by making Blueprint children from the provided C++ classes.
- Do not commit `Binaries`, `Intermediate`, `Saved`, or derived data.
- Run `./scripts/Run-Validation.ps1` after code changes. It checks that no English contractions are present in live C++ or configuration text before and after each build run, and it builds the editor target when Unreal is available.
- Update this README, [docs/DEVELOPMENT_LOG.md](docs/DEVELOPMENT_LOG.md), and [docs/HANDOFF.md](docs/HANDOFF.md) in the same change whenever project state changes.

## Next scoped increment

Create `Maps/PrototypeLandscape`, replace the runtime floor with an authored landscape and cover, then add Supplies and Fuel. Base building, AI combat, and win conditions are intentionally deferred until that environment foundation is in place.

## Continuity

[docs/HANDOFF.md](docs/HANDOFF.md) is the authoritative next-session starting point. [docs/DEVELOPMENT_LOG.md](docs/DEVELOPMENT_LOG.md) is append-only and records completed work and verification.
