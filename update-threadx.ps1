# script to update Azure RTOS ThreadX from Microsoft official repo

$Response = Invoke-WebRequest https://api.github.com/repos/azure-rtos/threadx/releases/latest
$version = ($Response | ConvertFrom-Json).tag_name

Write-Host "The latest Azure RTOS ThreadX version is $version"

git clone https://github.com/azure-rtos/threadx
Set-Location threadx
git checkout -b $version $version
Set-Location ..

Write-Host "Copying files..."

Copy-Item -Force -Path "threadx/common/src/*" -Destination "src/"
Copy-Item -Force -Path "threadx/common/inc/*" -Destination "src/"

$arch_list = Get-Childitem -Path "ports/arch" -Directory

foreach($arch in $arch_list){
    $arch_name = $arch.Name

    Copy-Item -Force -Path "threadx/ports/$arch_name/gnu/src/*" -Destination "ports/arch/$arch_name/"
    Copy-Item -Force -Path "threadx/ports/$arch_name/gnu/inc/tx_port.h" -Destination "ports/arch/$arch_name/tx_port_$arch_name.h"

    (Get-Content -Path "ports/arch/$arch_name/tx_port_$arch_name.h" -Raw) -replace "TX_PORT_H", "TX_PORT_$arch_name`_H".ToUpper() | Set-Content -Path "ports/arch/$arch_name/tx_port_$arch_name.h"
}

Remove-Item -Force -Recurse -Path "threadx" 

Write-Host "Update done! You may need to manually update tx_initialize_low_level.S for each BSP"