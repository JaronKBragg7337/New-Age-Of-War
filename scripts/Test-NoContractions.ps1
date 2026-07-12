[CmdletBinding()]
param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot '..'))
)

$pattern = "(?i)\b(?:[a-z]+)'(?:t|re|ve|ll|d|m|s)\b"
$targets = @(
    Join-Path $ProjectRoot 'Source',
    Join-Path $ProjectRoot 'Config'
)

$matches = foreach ($target in $targets) {
    if (Test-Path $target) {
        Get-ChildItem -Path $target -Recurse -File -Include *.h,*.cpp,*.cs,*.ini |
            Select-String -Pattern $pattern
    }
}

if ($matches) {
    Write-Error 'English contractions were found in live code or configuration:'
    $matches | ForEach-Object { Write-Error ("{0}:{1}: {2}" -f $_.Path, $_.LineNumber, $_.Line.Trim()) }
    exit 1
}

Write-Host 'No English contractions found in Source or Config.'
