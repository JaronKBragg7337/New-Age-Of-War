# New Ages Of War

New Ages Of War is a near-future combined-arms game that combines real-time strategy command with direct first-person control. The Phase 1 MVP is implemented as a self-contained tactical test match in Unreal Engine 5.8.

## Phase 1 playable slice

- A checked-in 4×4-component Landscape battlefield with gentle elevation, a flat operations zone, and combined-arms cover lanes.
- Supplies and Fuel, starting resources, and two resource points that generate into the commander wallet.
- An on-screen build menu with grid placement, collision and slope validation, resource costs, rotation, cancellation, Barracks, and Drone Pad.
- Infantry, recon drone, and light armored vehicle classes with distinct health, movement, and weapon values.
- Enhanced Input shared between RTS command view and possessed first-person control.
- Smooth possession and death-return camera transitions, held automatic fire, and friendly-fire filtering.
- Five hostile units that acquire targets, advance without a NavMesh dependency, and attack friendly units or structures.
- A destroy-all-enemies objective with HUD progress and victory feedback.

Primitive meshes remain intentional placeholders. Gameplay systems are Blueprint-ready so content can replace them without rewriting the control loop.

## Run it

1. Open [NewAgeOfWar.uproject](NewAgeOfWar.uproject) with Unreal Engine 5.8.
2. Select a C++ development environment if Unreal prompts for one, then build the `NewAgeOfWarEditor` target.
3. Open `PrototypeLandscape` if it is not already loaded, then press Play in Editor.

### Controls

| Mode | Controls |
| --- | --- |
| RTS command view | `WASD` pan, hold right mouse and move to rotate, left-click a friendly unit to possess it |
| Build menu | `1` Barracks, `2` Drone Pad, `R` rotate preview, left-click place, `Escape` cancel |
| Possessed unit | `WASD` move, mouse look, hold left-click to fire, `Escape` return to RTS |
| Possession test | While possessed, press `K` to eliminate the unit. After a 0.65-second transition, click another surviving unit. |

## Architecture

The key classes are intentionally small and Blueprint-ready. See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for ownership and extension points.

| Class | Responsibility |
| --- | --- |
| `ANAWPlayerController` | Owns the RTS and first-person state machine, Enhanced Input, selection, and camera transitions. |
| `ANAWStrategicPawn` | Top-down command camera and panning. |
| `ANAWPossessableUnit` | Friendly unit base: FPS camera, movement, health, hitscan weapon, and death signal. |
| `ANAWInfantry`, `ANAWReconDrone`, `ANAWLightArmoredVehicle` | Tuned Phase 1 unit variants that can become Blueprint parents. |
| `UNAWResourceWalletComponent`, `ANAWResourcePoint` | Supplies, Fuel, spending, and timed map deposits. |
| `UNAWBuildingPlacementComponent`, `ANAWBuildableStructure` | Placement preview, validation, costs, and structure lifecycle. |
| `ANAWEnemyUnit`, `ANAWEnemyController` | Hostile target acquisition, movement, and attacks. |
| `ANAWMatchObjective`, `ANAWHUD` | Match progress, victory state, resources, controls, and status feedback. |
| `ANAWGameMode` | Assembles the complete Phase 1 test match on the authored Landscape. |

## Development rules

- Use UE 5.8 and Enhanced Input.
- Keep gameplay iteration Blueprint-first by making Blueprint children from the provided C++ classes.
- Do not commit `Binaries`, `Intermediate`, `Saved`, or derived data.
- Run `./scripts/Run-Validation.ps1` after code changes. It checks live text, builds the editor target, boots the authored map through BeginPlay with null rendering, and verifies Landscape, economy, and enemy-controller startup.
- Run `./scripts/Regenerate-PrototypeLandscape.ps1` only when deterministic terrain geometry must be rebuilt.
- Update this README, [docs/DEVELOPMENT_LOG.md](docs/DEVELOPMENT_LOG.md), and [docs/HANDOFF.md](docs/HANDOFF.md) in the same change whenever project state changes.

## Verified state

- `NewAgeOfWarEditor Win64 Development`: passed.
- `NewAgeOfWar Win64 Development`: passed.
- Authored-map null-RHI startup smoke: passed.
- Runtime startup assertions: Landscape active, Supplies 525, Fuel 265, five enemy controllers active.
- No-contractions audit and `git diff --check`: passed.

The next product increment is a presentation and tuning pass: production meshes, materials, effects, audio, authored Blueprint children, and hands-on balance testing. Helicopters, jets, large-army optimization, networking, and full campaign systems remain outside Phase 1.

## Continuity

[docs/HANDOFF.md](docs/HANDOFF.md) is the authoritative next-session starting point. [docs/DEVELOPMENT_LOG.md](docs/DEVELOPMENT_LOG.md) is append-only and records completed work and verification.
