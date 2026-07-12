# Contribution and continuity rules

## Every change

1. Keep the change inside its approved phase scope.
2. Update `README.md` if the player-facing state, controls, setup, or next step changes.
3. Append a dated item to `docs/DEVELOPMENT_LOG.md`.
4. Update `docs/HANDOFF.md` with exact next actions and unresolved verification.
5. Run `./scripts/Run-Validation.ps1` before handoff or commit.
6. Commit and push the verified change to GitHub before ending the session. GitHub is the shared source of truth for collaborating humans and agents.

## Live-code wording check

The project prohibits English contractions in C++ source and runtime configuration. The validation script fails on patterns such as `cannot` written as a contraction. Use expanded language in comments, UI text, and log messages. This guard is intentionally narrow: it scans `Source` and `Config`, not generated or third-party files.

## Unreal source-control rules

- Commit `.uasset` and `.umap` files with their matching source and documentation updates.
- Never commit build products or local editor state.
- Add large binary source art to Git LFS before committing it.
- Do not rename reflected C++ classes casually. Redirects and Blueprint migration are required once assets reference them.
