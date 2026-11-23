param(
    [ValidateSet('Check','Build','Prepare','Test','Run','Package')][string]$Action = 'Check',
    [ValidateSet('Win64','Android')][string]$Platform = 'Win64',
    [ValidateSet('Menu','Match','Simulator')][string]$Mode = 'Menu'
)
$ErrorActionPreference = 'Stop'
$repoPath = Split-Path $PSScriptRoot -Parent
$projectPath = Join-Path $repoPath 'native/Aquila/Aquila.uproject'
$enginePath = 'C:/Program Files/Epic Games/UE_5.8'
$buildVersion = Get-Content -LiteralPath "$enginePath/Engine/Build/Build.version" -Raw | ConvertFrom-Json
if ($buildVersion.MajorVersion -ne 5 -or $buildVersion.MinorVersion -ne 8 -or $buildVersion.PatchVersion -ne 4 -or
    !(Test-Path -LiteralPath "$enginePath/Engine/Build/InstalledBuild.txt")) {
    throw 'Aquila requires launcher Unreal 5.8.4. Source engines and upgrades need a deliberate project decision.'
}
$launcher = Get-Content -LiteralPath 'C:/ProgramData/Epic/UnrealEngineLauncher/LauncherInstalled.dat' -Raw | ConvertFrom-Json
if (!($launcher.InstallationList | Where-Object { $_.AppName -eq 'UE_5.8' -and $_.InstallLocation.TrimEnd('\') -eq $enginePath.Replace('/','\') })) {
    throw 'Pinned engine is not registered as the launcher installation.'
}
Write-Output "Aquila: launcher $($buildVersion.MajorVersion).$($buildVersion.MinorVersion).$($buildVersion.PatchVersion), CL $($buildVersion.Changelist)"
Write-Output "Project: $projectPath"
if ($Platform -eq 'Android' -and !(Test-Path -LiteralPath "$enginePath/Engine/Binaries/Android/UnrealGame.target")) {
    throw 'Android target support is missing. Add Android through this engine version in Epic Launcher Options, then verify its SDK.'
}
# Scope the separate Aquila SDK to this process and its children; never change machine/user settings.
$androidSdkPath = Join-Path $repoPath '.runtime/android-sdk'
if (Test-Path -LiteralPath "$androidSdkPath/ndk/27.2.12479018/source.properties") {
    $env:ANDROID_HOME = $androidSdkPath
    $env:ANDROID_SDK_ROOT = $androidSdkPath
    $env:NDKROOT = "$androidSdkPath/ndk/27.2.12479018"
    $env:NDK_ROOT = $env:NDKROOT
    $env:ANDROID_NDK_ROOT = $env:NDKROOT
    $env:JAVA_HOME = 'C:/Program Files/Android/openjdk/jdk-21.0.8'
    $env:ANDROID_USER_HOME = Join-Path $repoPath '.runtime/android-user'
    $env:GRADLE_USER_HOME = Join-Path $repoPath '.runtime/gradle'
}
if ($Platform -eq 'Android') {
    foreach ($requiredPath in @("$env:ANDROID_HOME/platforms/android-36/android.jar", "$env:ANDROID_HOME/build-tools/36.0.0/aapt2.exe",
        "$env:NDKROOT/toolchains/llvm/prebuilt/windows-x86_64/bin/clang++.exe", "$env:JAVA_HOME/bin/java.exe")) {
        if (!(Test-Path -LiteralPath $requiredPath)) { throw "Android toolchain file missing: $requiredPath" }
    }
    Write-Output 'Android required files present. A successful package and physical-device run remain separate checks.'
}
if ($Action -eq 'Check') { return }
$editorPath = "$enginePath/Engine/Binaries/Win64/UnrealEditor.exe"
$commandletPath = "$enginePath/Engine/Binaries/Win64/UnrealEditor-Cmd.exe"
if ($Action -in @('Build','Package')) {
    $ours = Get-CimInstance Win32_Process -Filter "Name='UnrealEditor.exe' OR Name='UnrealEditor-Cmd.exe'" |
        Where-Object { $_.CommandLine -and $_.CommandLine.Replace('\','/').Contains($projectPath.Replace('\','/')) }
    if ($ours) { throw 'Close the Aquila editor before building. Other projects remain untouched.' }
}
switch ($Action) {
    'Build' {
        $targetName = if ($Platform -eq 'Android') { 'Aquila' } else { 'AquilaEditor' }
        & "$enginePath/Engine/Build/BatchFiles/Build.bat" $targetName $Platform Development "-Project=$projectPath" -WaitMutex -NoHotReloadFromIDE
    }
    'Test' {
        & $commandletPath $projectPath -unattended -NullRHI -nosound '-ExecCmds=Automation RunTests Aquila.' '-TestExit=Automation Test Queue Empty' "-ReportExportPath=$repoPath/.runtime/native-tests" -stdout -FullStdOutLogOutput
    }
    'Prepare' {
        & $commandletPath $projectPath -run=AquilaContent -unattended -NullRHI -stdout -FullStdOutLogOutput
    }
    'Run' {
        & $editorPath $projectPath /Engine/Maps/Entry -game -windowed -ResX=1600 -ResY=900 -NoSplash "-AquilaMode=$Mode"
    }
    'Package' {
        if (!(Test-Path -LiteralPath "$repoPath/native/Aquila/Content/Foundation/M_Planet.uasset")) {
            throw 'Generated project content is missing. Run Build, then Prepare, before packaging.'
        }
        [string[]]$platformOptions = if ($Platform -eq 'Android') { @('-cookflavor=ASTC') } else { @() }
        & "$enginePath/Engine/Build/BatchFiles/RunUAT.bat" BuildCookRun "-project=$projectPath" -noP4 "-platform=$Platform" @platformOptions -clientconfig=Development -build -cook -stage -pak -package -archive "-archivedirectory=$repoPath/.runtime/native-package/$Platform" -utf8output
    }
}
if ($LASTEXITCODE -ne 0) { throw "Native $Action failed: $LASTEXITCODE" }
if ($Action -eq 'Package' -and $Platform -eq 'Android') {
    $nativeApkPath = Join-Path $repoPath '.runtime/native-package/Android/Aquila-arm64.apk'
    $nativeApkArchive = [IO.Compression.ZipFile]::OpenRead($nativeApkPath)
    try {
        $nativePayload = $nativeApkArchive.GetEntry('assets/main.obb.png')
        if (!$nativePayload -or $nativePayload.Length -eq 0) { throw 'Android archive lacks embedded cooked game content.' }
    } finally { $nativeApkArchive.Dispose() }
    & "$env:ANDROID_HOME/build-tools/36.0.0/apksigner.bat" verify $nativeApkPath
    if ($LASTEXITCODE -ne 0) { throw 'Android APK signature verification failed.' }
    Write-Output 'Android APK contains cooked game content and its signature verifies. Physical-device validation remains required.'
}
