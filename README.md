# Getting started sample with Audio Codec on Mbed OS

This is an example to play and/or record then play PCM using Audio Codec with [NuMaker Mbed Audio](https://github.com/OpenNuvoton/NuMaker-mbed-Audio) library.

### CLI
Target board "NUMAKER_PFM_NUC472".
1. mbed target "NUMAKER_PFM_NUC472"
2. mbed toolchain "GCC_ARM"
3. mbed compile

Target board "NUMAKER_PFM_M453".
1. mbed target "NUMAKER_PFM_M453"
2. mbed toolchain "GCC_ARM"
3. mbed compile

Target board "NUMAKER_PFM_M487".
1. mbed target "NUMAKER_PFM_M487"
2. mbed toolchain "GCC_ARM"
3. mbed compile

Please note that the pinmap of M487 Mbed board between V1.2 and V2.0.
M487 pinmap setting of SD and I2C of mbed_app.json:

        V1.2 V2.0
-----------------
SD_DAT3 PE_5 PB_5
SD_DAT2 PE_4 PB_4

        V1.2 V2.0
-----------------
I2C_SDA PD_8 PD_0
I2C_SCL PD_9 PD_1

Target board "NUMAKER_PFM_NANO130".
1. mbed target "NUMAKER_PFM_NANO130"
2. mbed toolchain "GCC_ARM"
3. mbed compile

Target board "NUMAKER_IOT_M487".
1. mbed target "NUMAKER_IOT_M487"
2. mbed toolchain "GCC_ARM"
3. mbed compile

Please note that to connect headphone and microphone or headset to hear the loopback result.

Demo condition:
8000Hz stereo 16-bit



### Burn Code & Execute
1. Connect the board NuMaker-PFM-XXX with your PC by USB cable, then there will be one "MBED" disk.
2. Copy the built binary file into "MBED" disk on you PC.
3. Press reset button to execute, user could test Audio Codec.
