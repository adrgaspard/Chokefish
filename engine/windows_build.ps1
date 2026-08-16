param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("Debug", "Release")]
    [string]$Configuration
)

$ErrorActionPreference = "Stop"

$EngineDir = $PSScriptRoot
$BuildDir = Join-Path $EngineDir "build-windows"

$VsWhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $VsWhere))
{
    throw "vswhere.exe not found: Visual Studio with C++ tools is required"
}

$InstallationPath = & $VsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $InstallationPath)
{
    throw "No Visual Studio installation with C++ tools found"
}

$Cmake = Join-Path $InstallationPath "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if (-not (Test-Path $Cmake))
{
    $Cmake = "cmake"
}

$GeneratorMap = @{
    "15" = "Visual Studio 15 2017"
    "16" = "Visual Studio 16 2019"
    "17" = "Visual Studio 17 2022"
    "18" = "Visual Studio 18 2026"
}
$MajorVersion = (& $VsWhere -latest -products * -property installationVersion).Split(".")[0]
if (-not $GeneratorMap.ContainsKey($MajorVersion))
{
    throw "Unsupported Visual Studio version '$MajorVersion'"
}

& $Cmake -S $EngineDir -B $BuildDir -G $GeneratorMap[$MajorVersion] -A x64
if ($LASTEXITCODE -ne 0)
{
    exit $LASTEXITCODE
}
& $Cmake --build $BuildDir --config $Configuration
exit $LASTEXITCODE
