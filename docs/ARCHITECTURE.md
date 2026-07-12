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

## Intentional Phase 1 limits

- The test arena is C++ runtime geometry, not the final authored landscape map.
- Weapons are hitscan placeholders; there is no ammunition, reload, or effects layer yet.
- There is no unit command queue, AI, resources, structures, or win condition in this first vertical slice.
- The code targets single-player PIE. Networking must use server-authoritative possession and replicated health before multiplayer work starts.

## Future module boundaries

- `Economy`: Supplies, Fuel, gatherers, storage, and spending interface.
- `Construction`: build catalog, placement validation, ghost preview, and structures.
- `Combat`: team interface, weapons, damage, AI perception, and objectives.
- `World`: landscape, points of interest, navigation, and streaming.

Keep `ANAWPlayerController` focused on local input and view state. Put future strategy commands into a dedicated commander component or command subsystem rather than expanding the controller into an all-purpose system.
