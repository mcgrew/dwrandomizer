Function Get-FileName($initialDirectory)
{
    [System.Reflection.Assembly]::LoadWithPartialName("System.windows.forms") | Out-Null

    $OpenFileDialog = New-Object System.Windows.Forms.OpenFileDialog
    $OpenFileDialog.initialDirectory = $initialDirectory
    $OpenFileDialog.filter = "NES ROM (*.nes)| *.nes"
    $OpenFileDialog.ShowDialog() | Out-Null
    $OpenFileDialog.filename
}

Function Get-Folder($initialDirectory)
{
    [System.Reflection.Assembly]::LoadWithPartialName("System.windows.forms") | Out-Null

    $OpenFileDialog = New-Object System.Windows.Forms.FolderBrowserDialog
    $OpenFileDialog.initialDirectory = $initialDirectory
    $OpenFileDialog.filter = "NES ROM (*.nes)| *.nes"
    $OpenFileDialog.ShowDialog() | Out-Null
    $OpenFileDialog.SelectedPath
}

Write-Host "Choose the ROM File"
$inputfile = Get-FileName "C:"

$flags = read-host "Enter flags (leave blank for default)"
$seed = read-host "Enter seed number"

Write-Host "Choose the Output Folder"
$outputdir = Get-Folder "C:"

dwrandomizer.exe $inputfile $flags $seed $outputdir

Write-Host "Press any key to continue..."
cmd /c pause | out-null
