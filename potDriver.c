#include "potDriver.h"

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
#define TIME_FETCH_INTERVAL_MS 500

static pthread_t potThreadID;

int getVoltage0Reading()
{
    // Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
    if (!f) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        fclose(f);
        exit(-1);
    }
    // Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        fclose(f);
        exit(-1);
    }
    // Close file
    fclose(f);
    return a2dReading;
}

static void* getPotValue(void* args) {

    while(true)
    {
        int val = getVoltage0Reading();
        int vol = ((float)val / A2D_MAX_READING) * 100;
        printf("volume %d\n", vol);
        General_sleepForMs(TIME_FETCH_INTERVAL_MS);
        if(abs(AudioMixer_getVolume() - vol) > 5)
        {
            AudioMixer_setVolume(vol);
        }
    }

	return NULL;
}// getPotValue()

void potDriver_init(void) {

    pthread_create(&potThreadID, NULL, &getPotValue, NULL);
}

void potDriver_shutdown(void) {
    
    pthread_join(potThreadID, NULL);
}