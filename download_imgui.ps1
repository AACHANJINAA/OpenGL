$url = "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.9.zip"
$outputZip = Join-Path -Path $PSScriptRoot -ChildPath "imgui_temp.zip"
$extractPath = Join-Path -Path $PSScriptRoot -ChildPath "imgui_temp_extracted"

# 1. Download Zip
Write-Host "Downloading Dear ImGui v1.90.9..."
Invoke-WebRequest -Uri $url -OutFile $outputZip

# 2. Extract Zip
Write-Host "Extracting Zip..."
if (Test-Path $extractPath) { Remove-Item -Recurse -Force $extractPath }
Expand-Archive -Path $outputZip -DestinationPath $extractPath

# 3. Create target directories
$targetRoot = Join-Path -Path $PSScriptRoot -ChildPath "ThirdParty\imgui"
$targetBackends = Join-Path -Path $targetRoot -ChildPath "backends"

if (!(Test-Path $targetRoot)) { New-Item -ItemType Directory -Path $targetRoot }
if (!(Test-Path $targetBackends)) { New-Item -ItemType Directory -Path $targetBackends }

$extractedFolder = Get-ChildItem -Path $extractPath | Select-Object -First 1
$extractedFullPath = $extractedFolder.FullName

# 4. Copy core files
Write-Host "Copying ImGui core files..."
$coreFiles = @(
    "imconfig.h", "imgui.h", "imgui.cpp", "imgui_draw.cpp", 
    "imgui_internal.h", "imgui_tables.cpp", "imgui_widgets.cpp", 
    "imstb_rectpack.h", "imstb_textedit.h", "imstb_truetype.h"
)
foreach ($file in $coreFiles) {
    Copy-Item -Path (Join-Path $extractedFullPath $file) -Destination $targetRoot -Force
}

# 5. Copy backend files
Write-Host "Copying backend files (GLFW & OpenGL3)..."
$backendFiles = @(
    "backends\imgui_impl_glfw.h", "backends\imgui_impl_glfw.cpp",
    "backends\imgui_impl_opengl3.h", "backends\imgui_impl_opengl3.cpp",
    "backends\imgui_impl_opengl3_loader.h"
)
foreach ($file in $backendFiles) {
    Copy-Item -Path (Join-Path $extractedFullPath $file) -Destination $targetBackends -Force
}

# 6. Cleanup
Write-Host "Cleaning up temporary files..."
Remove-Item -Path $outputZip -Force
Remove-Item -Path $extractPath -Recurse -Force

Write-Host "Dear ImGui v1.90.9 successfully set up at ThirdParty/imgui!"
