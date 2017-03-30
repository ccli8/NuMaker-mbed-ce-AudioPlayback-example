#include "mbed.h"

#include "NAU8822L.h"

#if defined(TARGET_NUMAKER_PFM_NUC472)
#include "FATFileSystem.h"
#include "NuSDBlockDevice.h"
#include <stdio.h>
#include <errno.h>

NAU8822L audio(PC_9, PA_15, 0x1A, PG_7, PG_8, PG_9, PI_11, PI_12); // NAU8822L object
DigitalOut hp_enable(PH_2);
NuSDBlockDevice bd(MBED_CONF_APP_SD_DAT0, MBED_CONF_APP_SD_DAT1, MBED_CONF_APP_SD_DAT2, MBED_CONF_APP_SD_DAT3,  // SD DAT0-3
    MBED_CONF_APP_SD_CMD, MBED_CONF_APP_SD_CLK, MBED_CONF_APP_SD_CD);                                           // SD CMD/CLK/CD
FATFileSystem fs("fs");

#elif defined(TARGET_NUMAKER_PFM_M453)
NAU8822L audio(PD_4, PD_5, 0x1A, PA_5, PA_6, PA_7, PD_0, PA_4); // NAU8822L object
DigitalOut hp_enable(PE_1);

#endif

InterruptIn button(SW2);    // button SW2
DigitalOut led(LED1);       // flashing LED1(rgbled1)

int audioBuf[4096];

int readPtr = 0;
int writePtr = 0;

int samplingRate = 8000;
char channelCount = 1;
char sampleBitLength = 16;

#if defined(TARGET_NUMAKER_PFM_NUC472)
FILE *fd;

int theta = 0;

void return_error(int ret_val) {
    if (ret_val)
        printf("Failure. %d\r\n", ret_val);
    else
        printf("done.\r\n");
}

void errno_error(void* ret_val) {
    if (ret_val == NULL)
        printf(" Failure. %d \r\n", errno);
    else
        printf(" done.\r\n");
}

#elif defined(TARGET_NUMAKER_PFM_M453)
// 1k sine wave@sampling rate 8kHz mono 16-bit
const char sine1k[16] = {
	0x00, 0x00, 0x50, 0x0B, 0xFF, 0x0F, 0x50, 0x0B, 0x00, 0x00, 0xB0, 0xF4, 0x01, 0xF0, 0xB0, 0xF4
};

int flag = 0;

#endif

void flip(void) {
    led = !led;
}

void play(void) {
#if defined(TARGET_NUMAKER_PFM_NUC472)
    audio.write(audioBuf, readPtr, 4);
    readPtr += 4;
    readPtr &= 0xFFF;
    theta -= 4;
#elif defined(TARGET_NUMAKER_PFM_M453)
    if (flag == 0) {
        audio.write((int *)sine1k, readPtr, 2);
        readPtr += 2;
        readPtr &= 0x3;
    } else {
        audio.write(audioBuf, readPtr, 2);
        readPtr += 2;
        readPtr &= 0xFFF;
    }
#endif
}

void record(void) {
    audio.read();
    
#if defined(TARGET_NUMAKER_PFM_NUC472)
    audioBuf[writePtr] = audio.rxBuffer[0];
    audioBuf[++writePtr] = audio.rxBuffer[1];
    audioBuf[++writePtr] = audio.rxBuffer[2];
    audioBuf[++writePtr] = audio.rxBuffer[3];
    ++writePtr;
    theta += 4;
    if (writePtr > 4094) {
        writePtr = 0;
    }
#elif defined(TARGET_NUMAKER_PFM_M453)
    audioBuf[writePtr] = audio.rxBuffer[0];
    audioBuf[++writePtr] = audio.rxBuffer[1];
    ++writePtr;
    if (writePtr > 4094) {
        writePtr = 0;
        flag = 1;
    }
#endif  
}

void fillAudioBuf(void) {
#if defined(TARGET_NUMAKER_PFM_NUC472)
    while (!feof(fd)) {
        if (theta < 4096) {
            // read 2 integers
            fread(&audioBuf[writePtr], 4, 2, fd);
            NVIC_DisableIRQ(I2S1_IRQn); // FIXME
            theta += 2;
            NVIC_EnableIRQ(I2S1_IRQn);  // FIXME
            writePtr += 2;
            writePtr &= 0xFFF;
        }
    }
#elif defined(TARGET_NUMAKER_PFM_M453)
    while (1) {
        Thread::wait(500);
    }
#endif
}

void drainAudioBuf(void) {
#if defined(TARGET_NUMAKER_PFM_NUC472)
    int i = 0;
    while (1) {
        if (theta > 512 ) {
            fwrite(&audioBuf[readPtr], 4, 128, fd);
            NVIC_DisableIRQ(I2S1_IRQn); // FIXME
            theta -= 128;
            NVIC_EnableIRQ(I2S1_IRQn);  // FIXME
            readPtr += 128;
            if (readPtr > 4094)
                readPtr = 0;
            
            i += 512;
        }
        
        /* record about 10 seconds PCM */
        if (i >= samplingRate*channelCount*sampleBitLength/8*10)
            break;
    }
#elif defined(TARGET_NUMAKER_PFM_M453)
    while (flag == 0) {
        Thread::wait(500);
    }
#endif
}

void demo_record(void) {
    for (int i = 0; i < 4096; i++) {
        audioBuf[i] = 0;
    }
    
#if defined(TARGET_NUMAKER_PFM_NUC472)
    printf("Opening a new file test.pcm");
    fd = fopen("/fs/test.pcm", "w");
    errno_error(fd);
#endif
    
    audio.attach(&record);
    audio.format(samplingRate, channelCount, sampleBitLength);
    
    printf("Start recording...\r\n");
    audio.record();
    
    drainAudioBuf();
    
#if defined(TARGET_NUMAKER_PFM_NUC472)
    fclose(fd);
#endif
    
    printf("Stop recording.\r\n");
    audio.stop();
}

void demo_play(void) {
#if defined(TARGET_NUMAKER_PFM_NUC472)
    printf("Opening file test.pcm read-only");
    fd = fopen("/fs/test.pcm", "r");
    errno_error(fd);
    
//    fseek(fd, 44, SEEK_SET);
    
    for (int i = 0; i < 4096; i++) {
        audioBuf[i] = 0;
    }
#endif
    
    // disable headphone
    hp_enable = 1;
    
    audio.attach(&play);
    audio.format(samplingRate, channelCount, sampleBitLength);
    
    // enable headphone
	hp_enable = 0;
    
    printf("Start playing...\r\n");
    audio.start();
    
    fillAudioBuf();
    
#if defined(TARGET_NUMAKER_PFM_NUC472)
    fclose(fd);
#endif
    
    printf("Stop playing.\r\n");
    audio.stop();
    
    // disable headphone
    hp_enable = 1;
}

int main(void) {
    led = 1;
    
    button.rise(&flip);
    
#if defined(TARGET_NUMAKER_PFM_NUC472)
    int error = 0;
    
    printf("Mounting the filesystem on \"/fs\" ");
    error = fs.mount(&bd);
    return_error(error);
#endif
    
    demo_record();
    
    demo_play();
    
    led = 0;
}
