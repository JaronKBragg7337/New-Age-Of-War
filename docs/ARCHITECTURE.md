# Phase 1 architecture

## Possession flow

```text
ANAWStrategicPawn
      | left-click friendly unit
      v
ANAWPlayerController::PossessUnit
      | Possess + camera blend
      v
ANAWPossessableUnit (first-person camera, movement, weapon)
      | health reaches zero
      v
ANAWPlayerController::HandleControlledUnitDeath
      | 0.65-second view transition
      v
ANAWPlayerController::ReturnToRTSView -> ANAWStrategicPawn
```

`ANAWPlayerController` retains the strategic pawn while it possesses a unit. The pawn remains in the world and is immediately repossessed after death or when `Escape` is pressed. This is the invariant that makes switching reliable.

## Blueprint extension points

Create Blueprint children of the three unit classes for production assets. Tune these inherited properties per unit:

- `MaxHealth`
- `WeaponDamage`, `WeaponRange`, and `FireCooldown`
- `Movement` speed and acceleration
- `VisualMesh` and `FirstPersonCamera` transforms

Override `OnUnitDeath` in Blueprints for particles, audio, UI, or a death animation. Do not make a child destroy itself before the controller has completed its return transition.

## Phase 1 gameplay systems

- `UNAWResourceWalletComponent` owns Supplies and Fuel on the player controller, so possession never resets the economy.
- `ANAWResourcePoint` deposits on timers into that wallet.
- `UNAWBuildingPlacementComponent` owns the active preview and performs cursor, grid, slope, overlap, and affordability checks.
- `ANAWBuildableStructure` supplies costs, health, friendly-base tagging, and Blueprint hooks. Barracks and Drone Pad are native children.
- `ANAWEnemyController` performs direct target search, steering, line-of-sight checks, and timed attacks without navigation data.
- `ANAWMatchObjective` evaluates destroy-all-enemies or survival rules and exposes Blueprint delegates.
- `ANAWHUD` renders a content-independent playable interface for the test match.

## Intentional Phase 1 limits

- The terrain is an authored Landscape, while cover and gameplay actors are assembled at runtime for deterministic testing.
- Weapons are hitscan placeholders; there is no ammunition, reload, or effects layer yet.
- There is no unit command queue, production queue, worker simulation, or advanced squad formation system.
- The code targets single-player PIE. Networking must use server-authoritative possession and replicated health before multiplayer work starts.

## Future module boundaries

- `Economy`: Supplies, Fuel, gatherers, storage, and spending interface.
- `Construction`: build catalog, placement validation, ghost preview, and structures.
- `Combat`: team interface, weapons, damage, AI perception, and objectives.
- `World`: landscape, points of interest, navigation, and streaming.

Keep `ANAWPlayerController` focused on local input and view state. Put future strategy commands into a dedicated commander component or command subsystem rather than expanding the controller into an all-purpose system.
