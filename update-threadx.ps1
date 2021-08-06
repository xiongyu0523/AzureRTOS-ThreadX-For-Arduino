# script to update Azure RTOS ThreadX from Microsoft official repo

$Response = Invoke-WebRequest https://api.github.com/repos/azure-rtos/threadx/releases/latest
$vesion = ($Response | ConvertFrom-Json).tag_name

Write-Host "The latest Azure RTOS ThreadX version is $vesion"

git clone https://github.com/azure-rtos/threadx
Set-Location threadx
git checkout -b $vesion $vesion
Set-Location ..

Write-Host "Copying files..."

Copy-Item -Force -Path "threadx/common/src/*" -Destination "src/"
Copy-Item -Force -Path "threadx/common/inc/*" -Destination "src/"

$arch_list = Get-Childitem -Path "ports/arch" -Directory

foreach($arch in $arch_list){

    Copy-Item -Force -Path "threadx/ports/$arch/gnu/src/*" -Destination "ports/arch/$arch/"
    Copy-Item -Force -Path "threadx/ports/$arch/gnu/inc/tx_port.h" -Destination "ports/arch/$arch/tx_port_$arch.h"

    (Get-Content -Path "ports/arch/$arch/tx_port_$arch.h" -Raw) -replace "TX_PORT_H", "TX_PORT_$arch.h".ToUpper() | Set-Content -Path "ports/arch/$arch/tx_port_$arch.h"
}

Remove-Item -Force -Recurse -Path "threadx" 

Write-Host "Updatge done! Need manually update tx_initialize_low_level.S for each bsp"