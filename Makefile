OUTFILE = alarmClock
OUTDIR = $(HOME)/cmpt433/public/myApps

CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBB -lpthread -lasound
CFILES =  alarmClock.c general.c joystick.c responses.c segDisplay.c timeController.c udp.c audioControl.c potDriver.c textToSpeech.c

all: alarmClock wav_files web_app

alarmClock: alarmClock.c
	$(CC_C) $(CFLAGS) $(CFILES) -o $(OUTDIR)/$(OUTFILE) $(LFLAGS)

clean:
	rm $(OUTDIR)/$(OUTFILE)

# # Copy the sound files, and the nodeJS server to the public directory.
wav_files::
	mkdir -p $(OUTDIR)/sounds/ 
	cp -R sounds/* $(OUTDIR)/sounds/ 

web_app:
	mkdir -p $(OUTDIR)/webapp/ 
	cp -R webapp/* $(OUTDIR)/webapp/ 
	cd $(OUTDIR)/webapp/ && npm install