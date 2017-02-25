## Installation

### Requirements

1. Eclipse CDT 
2. GNU ARM Plugins for Eclipse
2. GNU ARM toolchain
3. JLink drivers
4. Nordic Semiconductor Packs

You can find manual how to install above tools at http://gnuarmeclipse.github.io/

To install Nordic Packs, open Packs perspective in Eclipse and find Nordic Semiconductor packages. Install them. Make sure you use proper versions:

* nRF_BLE, nRF_Libraries: 3.0.1
* nRF_DeviceFamilyPack: 8.0.3
* nRF_SoftDevice_Common: 1.1.4
* nRF_SoftDevice: 8.0.2

Other versions may not work, and for sure you will have to change include paths in project.

WARNING: Project does not work on JLink versions different than 5.02c. I dont know why.

### Preparing workspace
1. Clone project into your Eclipse workspace and import it.
2. Open Eclipse, then Window->Preferences->C/C++->Build->Environment. Create variable NordicSemiconductorPackPath pointing to your Packages/NordicSemiconductor folder. For me, this variable has value `/mnt/win/workspace/Packages/NordicSemiconductor`
3. Copy file `softdevice_handler.c` from `{your packs folder}/NordicSemiconductor/nRF_SoftDevice_Common/1.1.4/softdevice_handler` to `libs` folder in project. This file may not be included in repo due to Nordic licensing. If you know more elegant way to include it without copying, pull request is welcome.
4. Try to build project. If it suceeds, you're very happy. Proceed to 6. 
5. If project does not build, right click on project, open Properties->C/C++ Build->Paths And Symbols->Includes. Make sure every include item points to existing directory. Revise your environmental variables from point 2 if neccessary. 
6. If project builds, you can try to debug. Create new debug config (GDB Segger J-Link Debugging) which uses hex file:  `Debug/location_tag.hex`. Connect your beacon and start debugging.

### Flashing softdevice

Download and install nRF command line tools. For flashing SoftDevice, use nrfjprog tool:

```
./nrfjprog --family NRF51 --program s110_softdevice.hex --chiperase --verify --jdll /opt/SEGGER/JLink/libjlinkarm.so.5.2.3 
```

You can find `s110_softdevice.hex` in your packs folder, in `NordicSemiconductor/nRF_SoftDevice_S110/8.0.2/hex`.

Option `-jdll` is for forcing use of JLink 5.02c libs instead of JLink 6 ones, which flash the chip wrong (I don't know why, is a matter of config). Of course you have to give path to your installation of JLink, which may differ.

## Troubleshooting

#### Program hangs in strange address instead of going to main function

Probably you have not installed softdevice or accidentally flashed devboard without softdevice





