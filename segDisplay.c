// [SOURCE]: fputs() and fflush(): www.crasseux.com/books/ctutorial/fputs.html

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>

#include "segDisplay.h"
#include "timeController.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"
#define I2C_DEVICE_ADDRESS 0x20
#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

#define CONFIG_PIN_P9_18 "config-pin P9_18 i2c"
#define CONFIG_PIN_P9_17 "config-pin p9_17 i2c"
#define I2CSET_00_INIT "i2cset -y 1 0x20 0x00 0x00"
#define I2CSET_01_INIT "i2cset -y 1 0x20 0x01 0x00"
#define GPIO_EXPORT_61 "echo 61 > /sys/class/gpio/export"
#define GPIO_EXPORT_44 "echo 44 > /sys/class/gpio/export"
#define GPIO_EXPORT_SLEEP_MS 300
#define GPIO_DIRECTION_61 "echo out > /sys/class/gpio/gpio61/direction"
#define GPIO_DIRECTION_44 "echo out > /sys/class/gpio/gpio44/direction"
#define GPIO_VALUE_61 "/sys/class/gpio/gpio61/value"
#define GPIO_VALUE_44 "/sys/class/gpio/gpio44/value"

#define MAX_ATTEMPTS 5
#define NUM_DIGITS 10
#define MAX_DIGIT_VALUE 9
#define MAX_DISPLAY_VALUE 99
#define MIN_DISPLAY_VALUE 0
#define BASE 10
#define ON "1"
#define OFF "0"
#define SWITCH_DIGIT_SLEEP_MS 5
#define DIP_FETCH_INTERVAL_MS 100

//**************************
//    GLOBAL VARIABLES
//**************************

// Seg display configuration -- *Bits[]: {0,1,2,3,4,5,6,7,8,9}
static int upperBits[NUM_DIGITS] = {0x86, 0x02, 0x0e, 0x06, 0x8a, 0x8c, 0x8c, 0x14, 0x8e, 0x8e};
static int lowerBits[NUM_DIGITS] = {0xa1, 0x80, 0x31, 0xb0, 0x90, 0xb0, 0xb1, 0x02, 0xb1, 0x90};
static FILE* pFileGpioPin61 = NULL;
static FILE* pFileGpioPin44 = NULL;
static pthread_t displayThreadID;
static pthread_t setDisplayThreadID;
static pthread_t timeValuesThreadID;
static pthread_mutex_t segDisplayMutex = PTHREAD_MUTEX_INITIALIZER;
static enum STATUS status = RUNNING;

static enum DISPLAY_MODE displayMode;
static int currentDisplayedNumber = 0;
static int leftDigit = 0;
static int rightDigit = 0;
static int i2cFileDesc = -1;

static int hours;
static int minutes;
static int seconds;

//**************************
//   PROTOTYPES (PRIVATE)
//**************************

static void bruteForceConfig(char* command);
static void configureDigits(void);
static void config_pins(void);
static void* display(void* args);
static void* setDisplayValues(void* args);
static int initI2cBus(char* bus, int address);
static void setNumber(int number);
static void writeI2cReg(unsigned char regAddr, unsigned char value);


//**************************
//    FUNCTIONS (PRIVATE)
//**************************

static void bruteForceConfig(char* command) {
	int exitCode = -1;
	int attempts = 0;

	while(exitCode != 0 && attempts < MAX_ATTEMPTS) {
		exitCode = General_runCommand(command);
		attempts++;
		if(exitCode != 0) {
			printf("[FAILED] '%s'....", command); 
			if(attempts < MAX_ATTEMPTS) {
				printf(" Trying again");
			} else {
				perror("Cannot execute successfully: ");
				printf("%s\n", command);
				exit(exitCode);
			}
			printf("\n");
		}
	}
	printf("[SUCCESS] %s with %d attempts\n", command, attempts);
}// bruteForceConfig()


static void configureDigits(void) {
	if(currentDisplayedNumber > MAX_DISPLAY_VALUE) {
		leftDigit = MAX_DIGIT_VALUE;
		rightDigit = MAX_DIGIT_VALUE;
	}else if(currentDisplayedNumber < MIN_DISPLAY_VALUE) {
		leftDigit = MIN_DISPLAY_VALUE;
		rightDigit = MIN_DISPLAY_VALUE;
	} else {
		leftDigit = currentDisplayedNumber / BASE;
		rightDigit = currentDisplayedNumber % BASE;
	}
}// configureDigits()


static void config_pins(void) {
	// config pins
	bruteForceConfig(CONFIG_PIN_P9_18);
	bruteForceConfig(CONFIG_PIN_P9_17);

	// set direction of both 8-bit ports
    bruteForceConfig(I2CSET_00_INIT);
    bruteForceConfig(I2CSET_01_INIT);

	// export pins for gpio
    bruteForceConfig(GPIO_EXPORT_61);
    bruteForceConfig(GPIO_EXPORT_44);

    // sleep until pins are exported
    General_sleepForMs(GPIO_EXPORT_SLEEP_MS);

	// set direction for pins
    bruteForceConfig(GPIO_DIRECTION_61);
    bruteForceConfig(GPIO_DIRECTION_44);

	printf("[SYSTEM] Pins exported successully...\n");
}// config_pins()


static void* display(void* args) {
	while(status == RUNNING) {
		// reset registers
		pthread_mutex_lock(&segDisplayMutex);
		writeI2cReg(REG_DIRA, 0x00);
		writeI2cReg(REG_DIRB, 0x00);

		// 1. Turn off both digits by driving a 0 to GPIO pins (61 & 44)
		fputs(OFF, pFileGpioPin61);
		fflush(pFileGpioPin61);
		fputs(OFF, pFileGpioPin44);
		fflush(pFileGpioPin44);

		// 2. Drive I2C GPIO extender to display pattern for left digit (must write to registers 0x14 and 0x15)
		writeI2cReg(REG_OUTA, lowerBits[leftDigit]);
		writeI2cReg(REG_OUTB, upperBits[leftDigit]);
		
		// 3. Turn on left digit via GPIO 61 set to a 1. Wait for 5ms
		fputs(ON, pFileGpioPin61);
		fflush(pFileGpioPin61);
		General_sleepForMs(SWITCH_DIGIT_SLEEP_MS);

		// 4. Turn off both digits by driving a 0 to GPIO pins (61 & 44)
		fputs(OFF, pFileGpioPin61);
		fflush(pFileGpioPin61);
		fputs(OFF, pFileGpioPin44);
		fflush(pFileGpioPin44);

		// 5. Drive I2C GPIO extender to display pattern for right digit (must write to registers 0x14 and 0x15)
		writeI2cReg(REG_OUTA, lowerBits[rightDigit]);
		writeI2cReg(REG_OUTB, upperBits[rightDigit]);

		// 6. Turn on the right digit via GPIO 44 set to 1. Wait for 5ms
		fputs(ON, pFileGpioPin44);
		fflush(pFileGpioPin44);
		
		pthread_mutex_unlock(&segDisplayMutex);
		General_sleepForMs(SWITCH_DIGIT_SLEEP_MS);
	}

	return NULL;
}// display()


static void* getTimeValues(void* args) {
	while(status == RUNNING) {
		hours = TimeController_getCurrentHours();
		minutes = TimeController_getCurrentMinutes();
		seconds = TimeController_getCurrentSeconds();

		General_sleepForMs(500);
	}

	return NULL;
}// getTimeValues()


static void* setDisplayValues(void* args) {
	while(status == RUNNING) {

		printf("[DEBUG] displayMode = %d\n", displayMode);

		if(displayMode == HOURS) {
			// TODO: set HH for 2 sec
			// display hour value
			setNumber(hours);
		} else if(displayMode == MINUTES) {
			// TODO: set MM for 2 sec
			// display minutes
			setNumber(minutes);
		} else if(displayMode == SECONDS) {
			// TODO: set SS for 2 sec
			// display seconds
			setNumber(seconds);
		} else {
			// will break out of current checks if display mode is not ALL
			if(displayMode == ALL) {
				setNumber(hours);
				General_sleepForMs(500);
			} 

			if(displayMode == ALL) {
				setNumber(minutes);
				General_sleepForMs(500);
			} 

			if(displayMode == ALL) {
				setNumber(seconds);
				General_sleepForMs(500);
			} 
		}

		displayMode = ALL;
		General_sleepForMs(500);
	}

	return NULL;
}// setDisplayValues()


static int initI2cBus(char* bus, int address) {
	i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}// initI2cBus()


static void setNumber(int number) {
	pthread_mutex_lock(&segDisplayMutex);
	currentDisplayedNumber = number;
	configureDigits();
	pthread_mutex_unlock(&segDisplayMutex);
}// SegDisplay_setNumber()


static void writeI2cReg(unsigned char regAddr, unsigned char value) {
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}// writeI2cReg()


//**************************
//    FUNCTIONS (PUBLIC)
//**************************

enum STATUS SegDisplay_getStatus(void) {
	pthread_mutex_lock(&segDisplayMutex);
	enum STATUS stat = status;
	pthread_mutex_unlock(&segDisplayMutex);

	return stat;
}//SegDisplay_getStatus()


void SegDisplay_init(void) {
	config_pins();
	i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	// Open the pin value files and leave open until end of program
	pFileGpioPin61 = fopen(GPIO_VALUE_61, "w");
	pFileGpioPin44 = fopen(GPIO_VALUE_44, "w");
	setNumber(0);

	displayMode = ALL;

	pthread_create(&timeValuesThreadID, NULL, &getTimeValues, NULL);
	pthread_create(&displayThreadID, NULL, &display, NULL);
	pthread_create(&setDisplayThreadID, NULL, &setDisplayValues, NULL);
}// SegDisplay_init()


void SegDisplay_setDisplayMode(enum DISPLAY_MODE value) {
	// pthread_mutex_lock(&segDisplayMutex);
	displayMode = value;
	// pthread_mutex_unlock(&segDisplayMutex);
}// SegDisplay_setDisplayMode()


void SegDisplay_setStatus(enum STATUS value) {
	pthread_mutex_lock(&segDisplayMutex);
	status = value;
	pthread_mutex_unlock(&segDisplayMutex);
}//SegDisplay_setStatus()


void SegDisplay_shutdown(void) {
	// turn off display
	fputs(OFF, pFileGpioPin61);
	fflush(pFileGpioPin61);
	fputs(OFF, pFileGpioPin44);
	fflush(pFileGpioPin44);

	pthread_join(displayThreadID, NULL);
	pthread_join(setDisplayThreadID, NULL);
	pthread_join(timeValuesThreadID, NULL);

	close(i2cFileDesc);

	// Close the pin value files that were opened at init
	fclose(pFileGpioPin61);
	fclose(pFileGpioPin44);
}// SegDisplay_shutdown()
