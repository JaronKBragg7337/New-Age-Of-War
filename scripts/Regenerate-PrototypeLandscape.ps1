[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$projectRoot = Resolve-Path (Join-Path $PSScriptRoot '..')
$editor = 'C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
if (-not (Test-Path $editor)) {
    throw "UE 5.8 editor command was not found at $editor."
}

& $editor (Join-Path $projectRoot 'NewAgeOfWar.uproject') `
    '-run=NAWCreatePrototypeMap' -unattended -nullrhi -nosound `
    '-log=CreatePrototypeLandscape.log'
if ($LASTEXITCODE -ne 0) {
    throw "Landscape generation failed with exit code $LASTEXITCODE."
}

$map = Join-Path $projectRoot 'Content\Maps\PrototypeLandscape.umap'
if (-not (Test-Path $map)) {
    throw 'Landscape generation completed without creating the expected map.'
}
Write-Host "Regenerated $map"
