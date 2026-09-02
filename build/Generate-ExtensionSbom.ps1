# Generate-ExtensionSbom.ps1
param (
    [Parameter(Mandatory = $true)]
    [string]$Version,

    [Parameter(Mandatory = $false)]
    [string]$TemplatePath = "template-sbom.cdx.json",

    [Parameter(Mandatory = $false)]
    [string]$ArtifactsDir = "artifacts"
)

$ErrorActionPreference = "Stop"

# 1. Vérification de l'existence du template
if (-not (Test-Path -Path $TemplatePath)) {
    throw "Le fichier template '$TemplatePath' est introuvable."
}

# 2. Emplacement de recherche des ZIPs
if (-not (Test-Path -Path $ArtifactsDir)) {
    throw "Le dossier d'artefacts '$ArtifactsDir' est introuvable."
}

# 3. Récupération des fichiers ZIP cibles
$zipFiles = Get-ChildItem -Path $ArtifactsDir -Filter "php_win32service*.zip"

if ($zipFiles.Count -eq 0) {
    Write-Warning "Aucun fichier correspondant à 'php_win32service*.zip' n'a été trouvé dans '$ArtifactsDir'."
    exit 0
}

# 4. Informations sur les sources (Code source de la release / version)
$SourceUrl = "https://github.com/win32service/win32service/archive/refs/tags/v${Version}.zip"
$TempSourceZip = [System.IO.Path]::GetTempFileName() + ".zip"

Write-Host "Téléchargement de l'archive source : $SourceUrl" -ForegroundColor Cyan
try {
    Invoke-WebRequest -Uri $SourceUrl -OutFile $TempSourceZip
    $SourceHash = (Get-FileHash -Path $TempSourceZip -Algorithm SHA256).Hash.ToLower()
    Write-Host "Hash SHA256 des sources calculé : $SourceHash" -ForegroundColor Green
}
finally {
    if (Test-Path -Path $TempSourceZip) {
        Remove-Item -Path $TempSourceZip -Force
    }
}

# 5. Lecture du contenu du template
$templateContent = Get-Content -Path $TemplatePath -Raw -Encoding UTF8

# 6. Traitement pour chaque archive ZIP d'extension compilée
foreach ($zipFile in $zipFiles) {
    Write-Host "`nTraitement de l'artefact : $($zipFile.Name)" -ForegroundColor Cyan

    # Calcul de l'UUID pour ce fichier SBOM
    $fileUuid = [guid]::NewGuid().ToString()

    # Calcul de l'URL de téléchargement de l'asset GitHub
    $distributionUrl = "https://github.com/win32service/win32service/releases/download/v${Version}/$($zipFile.Name)"

    # Calcul du Hash SHA256 du fichier ZIP local
    $distributionHash = (Get-FileHash -Path $zipFile.FullName -Algorithm SHA256).Hash.ToLower()

    # Replacement des placeholders
    $sbomContent = $templateContent `
        -replace '\{@version\}', $Version `
        -replace '\{@file_uuid\}', $fileUuid `
        -replace '\{@source_url\}', $SourceUrl `
        -replace '\{@source_hash\}', $SourceHash `
        -replace '\{@distribution_url\}', $distributionUrl `
        -replace '\{@distribution_hash\}', $distributionHash

    # Nom du fichier SBOM généré (remplacement de .zip par -sbom.cdx.json)
    $sbomFileName = [System.IO.Path]::GetFileNameWithoutExtension($zipFile.Name) + "-sbom.cdx.json"
    $outputPath = Join-Path -Path $zipFile.DirectoryName -ChildPath $sbomFileName

    # Écriture du fichier final en UTF-8
    Set-Content -Path $outputPath -Value $sbomContent -Encoding UTF8
    Write-Host "SBOM généré avec succès : $outputPath" -ForegroundColor Green
}
