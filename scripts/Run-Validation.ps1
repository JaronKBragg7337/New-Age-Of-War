[CmdletBinding()]
param(
    [switch]$SkipBuild,
    [switch]$SkipSmoke
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

& $buildScript NewAgeOfWarEditor Win64 Development (Join-Path $projectRoot 'NewAgeOfWar.uproject') '-WaitMutex' '-MaxParallelActions=1' '-NoUBA'
if ($LASTEXITCODE -ne 0) {
    throw "Unreal editor build failed with exit code $LASTEXITCODE."
}

& (Join-Path $PSScriptRoot 'Test-NoContractions.ps1') -ProjectRoot $projectRoot

if (-not $SkipSmoke) {
    $editor = Join-Path $engineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
    $project = Join-Path $projectRoot 'NewAgeOfWar.uproject'
    & $editor $project '/Game/Maps/PrototypeLandscape' -game -nullrhi -nosound -unattended `
        -NoSplash -NoPause '-ExecCmds=Quit' '-log=Phase1Smoke.log'
    if ($LASTEXITCODE -ne 0) {
        throw "Unreal runtime smoke test failed with exit code $LASTEXITCODE."
    }

    $smokeLog = Join-Path $projectRoot 'Saved\Logs\Phase1Smoke.log'
    if (-not (Test-Path $smokeLog)) {
        throw 'Unreal runtime smoke log was not created.'
    }
    if (Select-String -Path $smokeLog -Pattern 'Fatal error:|LogWindows: Error:|Assertion failed:') {
        throw 'Fatal runtime messages were found in the Unreal smoke log.'
    }
    if (-not (Select-String -Path $smokeLog -SimpleMatch 'Phase 1 arena ready')) {
        throw 'The runtime arena did not report successful startup.'
    }
    if (-not (Select-String -Path $smokeLog -SimpleMatch 'Battlefield source: authored Landscape')) {
        throw 'The authored Landscape was not active in the runtime smoke test.'
    }
    if (-not (Select-String -Path $smokeLog -SimpleMatch 'Resource generation activated: Supplies=525 Fuel=265 EnemiesControlled=5')) {
        throw 'The economy did not report successful startup.'
    }
    Write-Host 'UE 5.8 null-RHI runtime smoke test passed.'
}

Write-Host 'Live-code validation and UE 5.8 editor build passed.'
