[CmdletBinding(SupportsShouldProcess)]
param(
    [Parameter()]
    [string] $Repository = $env:GITHUB_REPOSITORY,

    [Parameter()]
    [string] $ManifestPath = (Join-Path (Split-Path $PSScriptRoot -Parent) "labels.json")
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($Repository)) {
    throw "Repository is required. Pass -Repository owner/repo or set GITHUB_REPOSITORY."
}

$resolvedManifest = Resolve-Path -LiteralPath $ManifestPath
$labels = Get-Content -LiteralPath $resolvedManifest -Raw | ConvertFrom-Json

if ($labels.Count -eq 0) {
    throw "Label manifest is empty: $resolvedManifest"
}

$names = [System.Collections.Generic.HashSet[string]]::new(
    [System.StringComparer]::OrdinalIgnoreCase
)

foreach ($label in $labels) {
    $name = [string] $label.name
    $color = ([string] $label.color).TrimStart("#")
    $description = [string] $label.description

    if ([string]::IsNullOrWhiteSpace($name)) {
        throw "Every label must have a non-empty name."
    }
    if (-not $names.Add($name)) {
        throw "Duplicate label name: $name"
    }
    if ($color -notmatch "^[0-9a-fA-F]{6}$") {
        throw "Label '$name' has invalid color '$color'. Use a six-digit hexadecimal value."
    }
    if ($description.Length -gt 100) {
        throw "Label '$name' has a description longer than 100 characters."
    }

    if ($PSCmdlet.ShouldProcess("$Repository label '$name'", "Create or update")) {
        & gh label create $name `
            --repo $Repository `
            --color $color `
            --description $description `
            --force

        if ($LASTEXITCODE -ne 0) {
            throw "Failed to create or update label '$name'."
        }
    }
}
