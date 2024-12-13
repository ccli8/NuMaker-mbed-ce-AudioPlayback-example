#include "mbed.h"

namespace mbed_nuvoton {}
using namespace mbed_nuvoton;

#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
#include "FATFileSystem.h"
#include "NuSDBlockDevice.h"
#include <stdio.h>
#include <errno.h>

NuSDBlockDevice bd(MBED_CONF_APP_SD_DAT0, MBED_CONF_APP_SD_DAT1, MBED_CONF_APP_SD_DAT2, MBED_CONF_APP_SD_DAT3,  // SD DAT0-3
    MBED_CONF_APP_SD_CMD, MBED_CONF_APP_SD_CLK, MBED_CONF_APP_SD_CD);                                           // SD CMD/CLK/CD
FATFileSystem fs("fs");
#endif

#if defined(TARGET_NUMAKER_PFM_NUC472)
#include "NAU8822L.h"

NAU8822L audio(MBED_CONF_APP_I2C_SDA, MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_ADDR, MBED_CONF_APP_I2S_DO,
    MBED_CONF_APP_I2S_DI, MBED_CONF_APP_I2S_BCLK, MBED_CONF_APP_I2S_MCLK, MBED_CONF_APP_I2S_LRCK); // NAU8822L object
DigitalOut hp_enable(PH_2);
#elif defined(TARGET_NUMAKER_PFM_M453)
#include "NAU8822L.h"

NAU8822L audio(MBED_CONF_APP_I2C_SDA, MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_ADDR, MBED_CONF_APP_I2S_DO,
    MBED_CONF_APP_I2S_DI, MBED_CONF_APP_I2S_BCLK, MBED_CONF_APP_I2S_MCLK, MBED_CONF_APP_I2S_LRCK); // NAU8822L object
DigitalOut hp_enable(PE_1);
#elif defined(TARGET_NUMAKER_PFM_M487)
#include "NAU88L25.h"

NAU88L25 audio(MBED_CONF_APP_I2C_SDA, MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_ADDR, MBED_CONF_APP_I2S_DO,
    MBED_CONF_APP_I2S_DI, MBED_CONF_APP_I2S_BCLK, MBED_CONF_APP_I2S_MCLK, MBED_CONF_APP_I2S_LRCK); // NAU88L25 object
DigitalOut hp_enable(PE_13);
#elif defined(TARGET_NUMAKER_PFM_NANO130)
#include "NAU88L25.h"

NAU88L25 audio(MBED_CONF_APP_I2C_SDA, MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_ADDR, MBED_CONF_APP_I2S_DO,
    MBED_CONF_APP_I2S_DI, MBED_CONF_APP_I2S_BCLK, MBED_CONF_APP_I2S_MCLK, MBED_CONF_APP_I2S_LRCK); // NAU88L25 object
DigitalOut hp_enable(LED2); //dummy function
#elif defined(TARGET_NUMAKER_IOT_M487)
#include "NAU88L25.h"

NAU88L25 audio(MBED_CONF_APP_I2C_SDA, MBED_CONF_APP_I2C_SCL, MBED_CONF_APP_I2C_ADDR, MBED_CONF_APP_I2S_DO,
    MBED_CONF_APP_I2S_DI, MBED_CONF_APP_I2S_BCLK, MBED_CONF_APP_I2S_MCLK, MBED_CONF_APP_I2S_LRCK); // NAU88L25 object
DigitalOut hp_enable(PE_13);
#endif

InterruptIn button(SW2);    // button SW2
DigitalOut led(LED1);       // flashing LED1(rgbled1)

#if defined(TARGET_NUMAKER_PFM_NANO130)
int audioBuf[128];
#else
int audioBuf[4096];
#endif

int readPtr = 0;
int writePtr = 0;

int samplingRate = 8000;
char channelCount = 2;
char sampleBitLength = 16;

#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
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

#elif defined(TARGET_NUMAKER_PFM_M453) || defined(TARGET_NUMAKER_PFM_NANO130)
// 1k sine wave@sampling rate 8kHz stereo 16-bit
const char sine1k[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x50, 0x0B, 0x50, 0x0B, 0x00, 0x08, 0x00, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0xB0, 0xF4, 0xB0, 0xF4, 0x00, 0xF8, 0x00, 0xF8
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
        audio.write((int *)sine1k, readPtr, 1);
        readPtr += 1;
        readPtr &= 0x7; // sine1k / 4 - 1
    } else {
        audio.write(audioBuf, readPtr, 1);
        readPtr += 1;
        readPtr &= 0xFFF;
    }
#elif defined(TARGET_NUMAKER_PFM_M487)
    audio.write(audioBuf, readPtr, 4);
    readPtr += 4;
    readPtr &= 0xFFF;
    theta -= 4;
#elif defined(TARGET_NUMAKER_PFM_NANO130)
    audio.write((int *)audioBuf, readPtr, 1);
    readPtr += 1;
    readPtr &= 0x7; // sine1k / 4 - 1
#elif defined(TARGET_NUMAKER_IOT_M487)
    audio.write(audioBuf, readPtr, 4);
    readPtr += 4;
    readPtr &= 0xFFF;
    theta -= 4;
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
#elif defined(TARGET_NUMAKER_PFM_M487)
    audioBuf[writePtr] = audio.rxBuffer[0];
    //audioBuf[++writePtr] = audio.rxBuffer[1];
    //audioBuf[++writePtr] = audio.rxBuffer[2];
    //audioBuf[++writePtr] = audio.rxBuffer[3];
    //audioBuf[++writePtr] = audio.rxBuffer[4];
    //audioBuf[++writePtr] = audio.rxBuffer[5];
    //audioBuf[++writePtr] = audio.rxBuffer[6];
    //audioBuf[++writePtr] = audio.rxBuffer[7];
    ++writePtr;
    theta += 1;
    if (writePtr > 4094) {
        writePtr = 0;
    }
#elif defined(TARGET_NUMAKER_PFM_NANO130)
    /* note
       due to SRAM size limitation
       just demo loopback
    */
#elif defined(TARGET_NUMAKER_IOT_M487)
    audioBuf[writePtr] = audio.rxBuffer[0];
    //audioBuf[++writePtr] = audio.rxBuffer[1];
    //audioBuf[++writePtr] = audio.rxBuffer[2];
    //audioBuf[++writePtr] = audio.rxBuffer[3];
    //audioBuf[++writePtr] = audio.rxBuffer[4];
    //audioBuf[++writePtr] = audio.rxBuffer[5];
    //audioBuf[++writePtr] = audio.rxBuffer[6];
    //audioBuf[++writePtr] = audio.rxBuffer[7];
    ++writePtr;
    theta += 1;
    if (writePtr > 4094) {
        writePtr = 0;
    }
#endif
}

void loopback(void) {
    audio.readwrite();
}

void fillAudioBuf(void) {
#if defined(TARGET_NUMAKER_PFM_NUC472)
    while (!feof(fd)) {
        if (theta < 4096) {
            // read 2 integers
            fread(&audioBuf[writePtr], 4, 2, fd);
            audio.lock();   // protect shared variable
            theta += 2;
            audio.unlock(); // protect shared variable
            writePtr += 2;
            writePtr &= 0xFFF;
        }
    }
#elif defined(TARGET_NUMAKER_PFM_M453)
    while (1) {
#if MBED_MAJOR_VERSION >= 6
	ThisThread::sleep_for(500ms);
#else
        Thread::wait(500);
#endif

        printf("fill\r\n");
    }
#elif defined(TARGET_NUMAKER_PFM_M487)
    while (!feof(fd)) {
        if (theta < 4096) {
            // read 2 integers
            fread(&audioBuf[writePtr], 4, 2, fd);
            audio.lock();   // protect shared variable
            theta += 2;
            audio.unlock(); // protect shared variable
            writePtr += 2;
            writePtr &= 0xFFF;
        }
    }
#elif defined(TARGET_NUMAKER_PFM_NANO130)
    while (1) {
#if MBED_MAJOR_VERSION >= 6
	ThisThread::sleep_for(500ms);
#else
        Thread::wait(500);
#endif
        printf("fill\r\n");
    }
#elif defined(TARGET_NUMAKER_IOT_M487)
    while (!feof(fd)) {
        if (theta < 4096) {
            // read 2 integers
            fread(&audioBuf[writePtr], 4, 2, fd);
            audio.lock();   // protect shared variable
            theta += 2;
            audio.unlock(); // protect shared variable
            writePtr += 2;
            writePtr &= 0xFFF;
        }
    }
#endif
}

void drainAudioBuf(void) {
#if defined(TARGET_NUMAKER_PFM_NUC472)
    int i = 0;
    while (1) {
        if (theta > 512 ) {
            fwrite(&audioBuf[readPtr], 4, 128, fd);
            audio.lock();   // protect shared variable
            theta -= 128;
            audio.unlock(); // protect shared variable
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
#if MBED_MAJOR_VERSION >= 6
	ThisThread::sleep_for(500ms);
#else
        Thread::wait(500);
#endif
    }
#elif defined(TARGET_NUMAKER_PFM_M487)
    int i = 0;
    while (1) {
        if (theta > 512 ) {
            fwrite(&audioBuf[readPtr], 4, 128, fd);
            audio.lock();   // protect shared variable
            theta -= 128;
            audio.unlock(); // protect shared variable
            readPtr += 128;
            if (readPtr > 4094)
                readPtr = 0;
            
            i += 512;
        }
        
        /* record about 10 seconds PCM */
        if (i >= samplingRate*channelCount*sampleBitLength/8*10)
            break;
    }
#elif defined(TARGET_NUMAKER_PFM_NANO130)
    while (flag == 0) {
#if MBED_MAJOR_VERSION >= 6
	ThisThread::sleep_for(500ms);
#else
        Thread::wait(500);
#endif
    }
#elif defined(TARGET_NUMAKER_IOT_M487)
    int i = 0;
    while (1) {
        if (theta > 512 ) {
            fwrite(&audioBuf[readPtr], 4, 128, fd);
            audio.lock();   // protect shared variable
            theta -= 128;
            audio.unlock(); // protect shared variable
            readPtr += 128;
            if (readPtr > 4094)
                readPtr = 0;
            
            i += 512;
        }
        
        /* record about 10 seconds PCM */
        if (i >= samplingRate*channelCount*sampleBitLength/8*10)
            break;
    }
#endif
}

void demo_record(void) {
    /* Init global varibles first */
    readPtr = 0;
    writePtr = 0;
#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    theta = 0;
#endif

#if defined(TARGET_NUMAKER_PFM_NANO130)
    for (int i = 0; i < 128; i++) {
#else
    for (int i = 0; i < 4096; i++) {
#endif
        audioBuf[i] = 0;
    }
    
#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    printf("Opening a new file test.pcm");
    fd = fopen("/fs/test.pcm", "w");
    errno_error(fd);
#endif
    
    audio.attach(&record);
    audio.format(samplingRate, channelCount, sampleBitLength);
    
    printf("Start recording...\r\n");
    audio.record();
    
    drainAudioBuf();
    
#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    fclose(fd);
#endif
    
    printf("Stop recording.\r\n");
    audio.stop();
}

void demo_play(void) {
    /* Init global varibles first */
    readPtr = 0;
    writePtr = 0;
#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    theta = 0;
#endif

#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    printf("Opening file test.pcm read-only");
    fd = fopen("/fs/test.pcm", "r");
    //fd = fopen("/fs/82.wav", "r");
    errno_error(fd);
    
    //fseek(fd, 44, SEEK_SET);
    
    for (int i = 0; i < 4096; i++) {
        audioBuf[i] = 0;
    }
#else
    for (int i = 0; i < 8; i++) {
        audioBuf[i] = (sine1k[i * 4 + 3] << 24) | (sine1k[i * 4 + 2] << 16) | (sine1k[i * 4 + 1] << 8) | (sine1k[i * 4 + 0] << 0);
        //printf("0x%08X\r\n", audioBuf[i]);
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
    
#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    fclose(fd);
#endif
    
    printf("Stop playing.\r\n");
    audio.stop();
    
    // disable headphone
    hp_enable = 1;
}

void demo_loopback(void) {
    /* Init global varibles first */
    readPtr = 0;
    writePtr = 0;
#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    theta = 0;
#endif

    // disable headphone
    hp_enable = 1;
    
    audio.attach(&loopback);
    audio.format(samplingRate, channelCount, sampleBitLength);
    
    // enable headphone
    hp_enable = 0;
    
    printf("Start loopback...\r\n");
    audio.loopback();
    
    while (1) {
#if MBED_MAJOR_VERSION >= 6
	ThisThread::sleep_for(500ms);
#else
        Thread::wait(500);
#endif
        printf("loopback\r\n");
    }
    
    printf("Stop loopback.\r\n");
    audio.stop();
    
    // disable headphone
    hp_enable = 1;
}

int main(void) {
    led = 1;
#ifdef MBED_MAJOR_VERSION
    printf("Mbed OS version %d.%d.%d\r\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif    
    button.rise(&flip);
    
#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    int error = 0;
    
    printf("Mounting the filesystem on \"/fs\" ");
    error = fs.mount(&bd);
    return_error(error);
#endif
    
    //demo_record();
    
    //demo_play();
    
    demo_loopback();
    
    led = 0;
}
