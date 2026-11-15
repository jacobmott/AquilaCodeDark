param([switch]$Apply)
$ErrorActionPreference = 'Stop'
$repo = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$stamp = [DateTime]::UtcNow.ToString('yyyyMMddTHHmmssZ')
$projectKey = $repo -replace '[^a-zA-Z0-9]', '-'
$shared = Join-Path $env:USERPROFILE 'ClaudeMemory/aquila-code-dark/memory'
$profiles = @('.claude', '.claude-content', '.claude-duskforge3')
$note = "# Aquila Code Dark shared memory`n`nProject records are authoritative for all agents and accounts.`nStart at $($repo.Replace('\','/'))/AGENTS.md and Docs/TaskRoutes.md.`nSelect the current task through HANDOFF.md; search knowledge/README.md and LESSONS.md by topic.`nWrap saves context. Explicit newsession also commits and pushes only session-owned work.`nDo not infer deployment, transfer, spend or resumption authority from old notes.`n"
$results = @()
foreach ($profile in $profiles) {
    $account = Join-Path $env:USERPROFILE $profile
    $projectsRoot = [IO.Path]::GetFullPath((Join-Path $account 'projects'))
    $project = [IO.Path]::GetFullPath((Join-Path $projectsRoot $projectKey))
    $link = Join-Path $project 'memory'
    if (-not $project.StartsWith($projectsRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw 'Project path escaped account' }
    $settingsPath = Join-Path $account 'settings.json'
    $before = [IO.File]::ReadAllText($settingsPath)
    $settings = $before | ConvertFrom-Json
    if (Test-Path -LiteralPath $link) {
        $item = Get-Item -Force -LiteralPath $link
        if ($item.LinkType -and [IO.Path]::GetFullPath($item.Target) -ne [IO.Path]::GetFullPath($shared)) { throw "Existing memory link targets another folder: $profile" }
        if (-not $item.LinkType -and @(Get-ChildItem -Force -LiteralPath $link).Count -gt 0) { throw "Existing notes need a reviewed merge: $profile" }
    }
    $adds = @("Read(~/ClaudeMemory/aquila-code-dark/memory/**)", "Edit(~/ClaudeMemory/aquila-code-dark/memory/**)", "Read(~/$profile/projects/$projectKey/memory/**)", "Edit(~/$profile/projects/$projectKey/memory/**)")
    if (-not $settings.permissions) { $settings | Add-Member permissions ([pscustomobject]@{}) }
    if (-not $settings.permissions.PSObject.Properties['allow']) { $settings.permissions | Add-Member allow @() }
    $settings.permissions.allow = @($settings.permissions.allow) + @($adds | Where-Object { $_ -notin $settings.permissions.allow })
    if ($Apply) {
        New-Item -ItemType Directory -Path $shared -Force | Out-Null
        $index = Join-Path $shared 'MEMORY.md'
        if (-not (Test-Path -LiteralPath $index)) { [IO.File]::WriteAllText($index, $note, [Text.UTF8Encoding]::new($false)) }
        New-Item -ItemType Directory -Path $project -Force | Out-Null
        if (Test-Path -LiteralPath $link) {
            $item = Get-Item -Force -LiteralPath $link
            if (-not $item.LinkType) {
                $preserved = [IO.Path]::GetFullPath((Join-Path $project "memory.pre-share-$stamp"))
                if (-not $preserved.StartsWith($project + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw 'Backup path escaped project' }
                Move-Item -LiteralPath $link -Destination $preserved
            }
        }
        if (-not (Test-Path -LiteralPath $link)) { New-Item -ItemType Junction -Path $link -Target $shared | Out-Null }
        if ([IO.File]::ReadAllText($settingsPath) -ne $before) { throw "Concurrent settings edit: $profile" }
        $after = $settings | ConvertTo-Json -Depth 100
        if (($before | ConvertFrom-Json | ConvertTo-Json -Depth 100) -ne $after) {
            Copy-Item -LiteralPath $settingsPath -Destination "$settingsPath.bak-aquila-$stamp"
            $tmp = "$settingsPath.aquila-$stamp.tmp"
            [IO.File]::WriteAllText($tmp, $after + "`n", [Text.UTF8Encoding]::new($false))
            [IO.File]::Move($tmp, $settingsPath, $true)
        }
        $item = Get-Item -Force -LiteralPath $link
        if ($item.LinkType -ne 'Junction' -or [IO.Path]::GetFullPath($item.Target) -ne [IO.Path]::GetFullPath($shared)) { throw 'Memory link verification failed' }
        $hash = (Get-FileHash -LiteralPath (Join-Path $link 'MEMORY.md') -Algorithm SHA256).Hash
    } else { $hash = $null }
    $results += [pscustomobject]@{ profile=$profile; link=$link; target=$shared; applied=[bool]$Apply; index_sha256=$hash }
}
if ($Apply -and @($results.index_sha256 | Select-Object -Unique).Count -ne 1) { throw 'Account memory hashes differ' }
$results | ConvertTo-Json -Depth 5
