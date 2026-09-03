# Generate-ExtensionSbom.ps1
param (
    [Parameter(Mandatory = $true)]
    [string]$Version,

    [Parameter(Mandatory = $true)]
    [string]$PhpVersion,

    [Parameter(Mandatory = $false)]
    [string]$TemplatePath = "template-sbom.cdx.json",

    [Parameter(Mandatory = $false)]
    [string]$ArtifactsDir = "artifacts"
)

# Fonction native .NET pour calculer le SHA256 d'un fichier
function Get-Sha256Native {
    param ([string]$Path)
    $stream = [System.IO.File]::OpenRead($Path)
    try {
        $sha256 = [System.Security.Cryptography.SHA256]::Create()
        $hashBytes = $sha256.ComputeHash($stream)
        return ([System.BitConverter]::ToString($hashBytes)).Replace('-', '').ToLower()
    }
    finally {
        $stream.Close()
        $stream.Dispose()
    }
}
$generatorVersion = "0.1.0"
$ErrorActionPreference = "Stop"

# 1. Vérification de l'existence du template
if (-not (Test-Path -Path $TemplatePath)) {
    throw "Unable to finc template file '$TemplatePath'"
}

# 2. Emplacement de recherche des ZIPs
if (-not (Test-Path -Path $ArtifactsDir)) {
    throw "Unable to func artifact folder '$ArtifactsDir'"
}

# 3. Récupération des fichiers ZIP cibles
$zipFiles = Get-ChildItem -Path $ArtifactsDir -Filter "php_win32service*.zip"

if ($zipFiles.Count -eq 0) {
    Write-Warning "No file for 'php_win32service*.zip' n'a été trouvé dans '$ArtifactsDir'."
    exit 0
}

# 4. Informations sur les sources (Code source de la release / version)
$SourceUrl = "https://github.com/win32service/win32service/archive/refs/tags/${Version}.zip"
$TempSourceZip = [System.IO.Path]::GetTempFileName() + ".zip"

Write-Host "Download sources archive : $SourceUrl" -ForegroundColor Cyan
try {
    Invoke-WebRequest -Uri $SourceUrl -OutFile $TempSourceZip
    $SourceHash = Get-Sha256Native -Path $TempSourceZip
    Write-Host "Source archive Hash SHA256 : $SourceHash" -ForegroundColor Green
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
    Write-Host "`nArtifact processing : $($zipFile.Name)" -ForegroundColor Cyan

    # Calcul de l'UUID pour ce fichier SBOM
    $fileUuid = [guid]::NewGuid().ToString()

    # Calcul de l'URL de téléchargement de l'asset GitHub
    $distributionUrl = "https://github.com/win32service/win32service/releases/download/${Version}/$($zipFile.Name)"

    # Calcul du Hash SHA256 du fichier ZIP local
    $distributionHash = Get-Sha256Native -Path $zipFile.FullName
    $dateNow = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")

    # Replacement des placeholders
    $sbomContent = $templateContent `
        -replace '\{@version\}', $Version `
        -replace '\{@file_uuid\}', $fileUuid `
        -replace '\{@source_url\}', $SourceUrl `
        -replace '\{@source_hash\}', $SourceHash `
        -replace '\{@distribution_url\}', $distributionUrl `
        -replace '\{@distribution_hash\}', $distributionHash `
        -replace '\{@date_now\}', $dateNow `
        -replace '\{@php_version\}', $PhpVersion

		$jsonObject = $sbomContent | ConvertFrom-Json
		if (-not $jsonObject.metadata) {
				$jsonObject | Add-Member -MemberType NoteProperty -Name "metadata" -Value ([PSCustomObject]@{})
		}

		$newToolComponent = [PSCustomObject]@{
		    purl = "pkg:generic/macintoshplus/sbom-generator@${generatorVersion}"
				type    = "application"
				publisher  = "Macintoshplus"
				group  = "macintoshplus"
				name    = "sbom-generator"
				version = $generatorVersion
				licenses = @(
                [PSCustomObject]@{
                    license = [PSCustomObject]@{
                        acknowledgement = "declared"
                        name            = "European Union Public License 1.2"
                        url             = "https://spdx.org/licenses/EUPL-1.2.html"
                    }
                }
            )
		}

		$jsonObject.metadata | Add-Member -MemberType NoteProperty -Name "tools" -Value ([PSCustomObject]@{
				components = @($newToolComponent)
		}) -Force
		$sbomContent = $jsonObject | ConvertTo-Json -Depth 100

    # Nom du fichier SBOM généré (remplacement de .zip par -sbom.cdx.json)
    $sbomFileName = [System.IO.Path]::GetFileNameWithoutExtension($zipFile.Name) + "-sbom.cdx.json"
    $outputPath = Join-Path -Path $zipFile.DirectoryName -ChildPath $sbomFileName

    # Écriture du fichier final en UTF-8
    Set-Content -Path $outputPath -Value $sbomContent -Encoding UTF8
    Write-Host "SBOM generated : $outputPath" -ForegroundColor Green
}
