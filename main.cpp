#include "mbed.h"

#include "NAU8822L.h"

#if defined(TARGET_NUMAKER_PFM_NUC472)
#include "NuSDFileSystem.h"

NAU8822L audio(PC_9, PA_15, 0x1A, PG_7, PG_8, PG_9, PI_11, PI_12); // NAU8822L object
DigitalOut hp_enable(PH_2);
NuSDFileSystem Nu_SD(PF_6, PF_7, PF_8, PF_5 ,PF_4, PF_3, PF_2, "sd");
#elif defined(TARGET_NUMAKER_PFM_M453)
NAU8822L audio(PD_4, PD_5, 0x1A, PA_5, PA_6, PA_7, PD_0, PA_4); // NAU8822L object
DigitalOut hp_enable(PE_1);
#endif

InterruptIn button(SW2);    // button SW2
DigitalOut led(LED1);       // flashing LED1(rgbled1)

int readPtr = 0;

#if defined(TARGET_NUMAKER_PFM_NUC472)
FILE *fp;

int audioBuf[4096];

int writePtr = 0;
int theta = 0;
#elif defined(TARGET_NUMAKER_PFM_M453)
// 1k sine wave@sampling rate 16kHz stereo 16-bit
const char sine1k[] = {
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x06, 0x1F, 0x06, 0x50, 0x0B, 0x50, 0x0B, 0xC7, 0x0E, 0xC7, 0x0E,
    0xFF, 0x0F, 0xFF, 0x0F, 0xC7, 0x0E, 0xC7, 0x0E, 0x50, 0x0B, 0x50, 0x0B, 0x1F, 0x06, 0x1F, 0x06,
    0x00, 0x00, 0x00, 0x00, 0xE1, 0xF9, 0xE1, 0xF9, 0xB0, 0xF4, 0xB0, 0xF4, 0x39, 0xF1, 0x39, 0xF1,
    0x01, 0xF0, 0x01, 0xF0, 0x39, 0xF1, 0x39, 0xF1, 0xB0, 0xF4, 0xB0, 0xF4, 0xE1, 0xF9, 0xE1, 0xF9
};
#endif

void flip(void) {
    led = !led;
}

void playback(void) {
#if defined(TARGET_NUMAKER_PFM_NUC472)
    audio.write(audioBuf, readPtr, 4);
    readPtr += 4;
    readPtr &= 0xFFF;
    theta -= 4;
#elif defined(TARGET_NUMAKER_PFM_M453)
    audio.write((int *)sine1k, readPtr, 2);
    readPtr += 2;
    readPtr &= 0xF;
#endif
}

void fillAudioBuf(void) {
#if defined(TARGET_NUMAKER_PFM_NUC472)
    while (!feof(fp)) {
        if (theta < 4096) {
            // read 2 integers
            fread(&audioBuf[writePtr], 4, 2, fp);
            NVIC_DisableIRQ(I2S1_IRQn); // FIXME
            theta += 2;
            NVIC_EnableIRQ(I2S1_IRQn);  // FIXME
            writePtr += 2;
            writePtr &= 0xFFF;
        }
    }
#elif defined(TARGET_NUMAKER_PFM_M453)
    while (1);
#endif
}

int main(void) {
    led = 1;
    
    // disable headphone
    hp_enable = 1;
    
    button.rise(&flip);
#if defined(TARGET_NUMAKER_PFM_NUC472)
    fp = fopen("/sd/test.pcm", "r");
    if (fp == NULL) {
        perror("Error opening file!\n");
        return -1;
    }
    
    for (int i = 0; i < 4096; i++) {
        audioBuf[i] = 0;
    }
#endif
    audio.attach(&playback);
    
    printf("Start playing...\n");
    audio.start();
    
    // enable headphone
	hp_enable = 0;
    
    fillAudioBuf();
    
    printf("Stop playing.\n");
    audio.stop();
    
    led = 0;
}
