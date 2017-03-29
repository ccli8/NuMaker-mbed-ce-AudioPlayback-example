# Getting started sample with Audio Codec on mbed OS

This is an example to play and record PCM using Audio Codec NAU8822L with [NuMaker mbed Audio](https://github.com/OpenNuvoton/NuMaker-mbed-Audio) library.

### mbed CLI
Play PCM file with "NUMAKER_PFM_NUC472".
Default is a 8kHz mono 16-bit PCM file.
1. mbed target "NUMAKER_PFM_NUC472"
2. mbed toolchain "GCC_ARM"
3. mbed compile

Play PCM buffer with "NUMAKER_PFM_M453".
Default is a 8kHz mono 16-bit PCM buffer.
1. mbed target "NUMAKER_PFM_M453"
2. mbed toolchain "GCC_ARM"
3. mbed compile

Please note that we need a headphone to hear the result.

### Burn Code & Execute
1. Connect the board NuMaker-PFM-XXX with your PC by USB cable, then there will be one "MBED" disk.
2. Copy the built binary file into "MBED" disk on you PC.
3. Press reset button to execute, user could test Audio Codec.
