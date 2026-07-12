[CmdletBinding()]
param(
    [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'
$projectRoot = Resolve-Path (Join-Path $PSScriptRoot '..')
& (Join-Path $PSScriptRoot 'Test-NoContractions.ps1') -ProjectRoot $projectRoot

if ($SkipBuild) {
    Write-Host 'Live-code validation passed. Editor build skipped by request.'
    exit 0
}

$engineRoot = 'C:\Program Files\Epic Games\UE_5.8'
$buildScript = Join-Path $engineRoot 'Engine\Build\BatchFiles\Build.bat'
if (-not (Test-Path $buildScript)) {
    throw "UE 5.8 build script was not found at $buildScript. Run with -SkipBuild or install UE 5.8."
}

& $buildScript NewAgeOfWarEditor Win64 Development (Join-Path $projectRoot 'NewAgeOfWar.uproject') '-WaitMutex' '-MaxParallelActions=2' '-NoUBA'
if ($LASTEXITCODE -ne 0) {
    throw "Unreal editor build failed with exit code $LASTEXITCODE."
}

& (Join-Path $PSScriptRoot 'Test-NoContractions.ps1') -ProjectRoot $projectRoot
Write-Host 'Live-code validation and UE 5.8 editor build passed.'
