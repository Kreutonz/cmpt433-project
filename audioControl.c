#define _DEFAULT_SOURCE
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <limits.h>
#include <alloca.h> // needed for mixer
#include <mpg123.h>
#include "audioControl.h"

static snd_pcm_t *handle;

#define DEFAULT_VOLUME 80

#define SAMPLE_RATE 44100
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (sizeof(short)) 			// bytes per sample
// Sample size note: This works for mono files because each sample ("frame') is 1 value.
// If using stereo files then a frame would be two samples.

static unsigned long playbackBufferSize = 0;
static short *playbackBuffer = NULL;


// Currently active (waiting to be played) sound bites
#define MAX_SOUND_BITES 1
typedef struct {
	// A pointer to a previously allocated sound bite (wavedata_t struct).
	// Note that many different sound-bite slots could share the same pointer
	// (overlapping cymbal crashes, for example)
	wavedata_t *pSound;

	// The offset into the pData of pSound. Indicates how much of the
	// sound has already been played (and hence where to start playing next).
	int location;
} playbackSound_t;

wavedata_t def_alarmData1;
wavedata_t def_alarmData2;
wavedata_t def_alarmData3;
wavedata_t cst_alarmData1;
wavedata_t cst_alarmData2;
wavedata_t rickrollData;
wavedata_t punjabiData;
wavedata_t soundData;

static playbackSound_t soundBites[MAX_SOUND_BITES];

// Playback threading
void* playbackThread(void* arg);
static bool stopping = false;
static bool isPlaying = false;
static bool isLooping = false;
static pthread_t playbackThreadId;
static pthread_mutex_t audioMutex = PTHREAD_MUTEX_INITIALIZER;

static int volume = 0;

void AudioController_init(void)
{
	AudioMixer_setVolume(DEFAULT_VOLUME);

	// Initialize the currently active sound-bites being played
	// REVISIT:- Implement this. Hint: set the pSound pointer to NULL for each
	//     sound bite.

	for (int i = 0; i < MAX_SOUND_BITES; i++) {
		soundBites[i].pSound = NULL;
		soundBites[i].location = 0;
	}


	// Open the PCM output
	int err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Configure parameters of PCM output
	err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			NUM_CHANNELS,
			SAMPLE_RATE,
			1,			// Allow software resampling
			50000);		// 0.05 seconds per buffer
	if (err < 0) {
		printf("Playback open error: %s\n", snd_strerror(err));
		exit(EXIT_FAILURE);
	}

	// Allocate this software's playback buffer to be the same size as the
	// the hardware's playback buffers for efficient data transfers.
	// ..get info on the hardware buffers:
 	unsigned long unusedBufferSize = 0;
	snd_pcm_get_params(handle, &unusedBufferSize, &playbackBufferSize);
	// ..allocate playback buffer:
	playbackBuffer = malloc(playbackBufferSize * sizeof(*playbackBuffer));

	AudioMixer_readWaveFileIntoMemory("sounds/alarm1.wav", &def_alarmData1);
	AudioMixer_readWaveFileIntoMemory("sounds/alarm2.wav", &def_alarmData2);
	AudioMixer_readWaveFileIntoMemory("sounds/alarm3.wav", &def_alarmData3);
	AudioMixer_readWaveFileIntoMemory("sounds/custom1.wav", &cst_alarmData1);
	AudioMixer_readWaveFileIntoMemory("sounds/custom2.wav", &cst_alarmData2);
	AudioMixer_readMP3FileIntoMemory("sounds/rickroll.mp3", &rickrollData);
	AudioMixer_readMP3FileIntoMemory("sounds/punjabi.mp3", &punjabiData);

	// Launch playback thread:
	pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
}

void AudioController_shutdown(void)
{
	AudioMixer_freeWaveFileData(&def_alarmData1);
	AudioMixer_freeWaveFileData(&def_alarmData2);
	AudioMixer_freeWaveFileData(&def_alarmData3);
	AudioMixer_freeWaveFileData(&cst_alarmData1);
	AudioMixer_freeWaveFileData(&cst_alarmData2);
	AudioMixer_cleanup();
}

// Client code must call AudioMixer_freeWaveFileData to free dynamically allocated data.
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound)
{
	assert(pSound);

	// The PCM data in a wave file starts after the header:
	const int PCM_DATA_OFFSET = 44;

	// Open the wave file
	FILE *file = fopen(fileName, "r");
	if (file == NULL) {
		fprintf(stderr, "ERROR: Unable to open file %s.\n", fileName);
		exit(EXIT_FAILURE);
	}

	// Get file size
	fseek(file, 0, SEEK_END);
	int sizeInBytes = ftell(file) - PCM_DATA_OFFSET;
	pSound->numSamples = sizeInBytes / SAMPLE_SIZE;

	// Search to the start of the data in the file
	fseek(file, PCM_DATA_OFFSET, SEEK_SET);

	// Allocate space to hold all PCM data
	pSound->pData = malloc(sizeInBytes);
	if (pSound->pData == 0) {
		fprintf(stderr, "ERROR: Unable to allocate %d bytes for file %s.\n",
				sizeInBytes, fileName);
		exit(EXIT_FAILURE);
	}

	// Read PCM data from wave file into memory
	int samplesRead = fread(pSound->pData, SAMPLE_SIZE, pSound->numSamples, file);
	if (samplesRead != pSound->numSamples) {
		fprintf(stderr, "ERROR: Unable to read %d samples from file %s (read %d).\n",
				pSound->numSamples, fileName, samplesRead);
		exit(EXIT_FAILURE);
	}
}

// [SOURCE]: https://stackoverflow.com/questions/19604216/mpg123-open-and-const
void AudioMixer_readMP3FileIntoMemory(const char *fileName, wavedata_t *pSound)
{
    assert(pSound);

    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err;

    int channels, encoding;
    long rate;

    mpg123_init();
    mh = mpg123_new(NULL, &err);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char *)malloc(buffer_size * sizeof(unsigned char));

    if (mpg123_open(mh, fileName) != MPG123_OK ||
        mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
    {
        fprintf(stderr, "Trouble with mpg123: %s\n", mpg123_strerror(mh));
        return;
    }

    if (encoding != MPG123_ENC_SIGNED_16)
    {
        fprintf(stderr, "Bad encoding: 0x%x!\n", encoding);
        return;
    }

    pSound->pData = NULL;
    pSound->numSamples = 0;

    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
    {
        pSound->pData = realloc(pSound->pData, (pSound->numSamples + done / 2) * sizeof(short));
        memcpy(pSound->pData + pSound->numSamples, buffer, done);
        pSound->numSamples += done / 2;
    }

    free(buffer);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
}


void AudioMixer_freeWaveFileData(wavedata_t *pSound)
{
	pSound->numSamples = 0;
	free(pSound->pData);
	pSound->pData = NULL;
}

void AudioMixer_queueSound(wavedata_t *pSound)
{
	// Ensure we are only being asked to play "good" sounds:
	assert(pSound->numSamples > 0);
	assert(pSound->pData);

	soundBites[0].pSound = NULL;
    soundBites[0].location = 0;

	// Insert the sound by searching for an empty sound bite spot
	/*
	 * REVISIT: Implement this:
	 * 1. Since this may be called by other threads, and there is a thread
	 *    processing the soundBites[] array, we must ensure access is threadsafe.
	 * 2. Search through the soundBites[] array looking for a free slot.
	 * 3. If a free slot is found, place the new sound file into that slot.
	 *    Note: You are only copying a pointer, not the entire data of the wave file!
	 * 4. After searching through all slots, if no free slot is found then print
	 *    an error message to the console (and likely just return vs asserting/exiting
	 *    because the application most likely doesn't want to crash just for
	 *    not being able to play another wave file.
	 */

	pthread_mutex_lock(&audioMutex);

	for (int i = 0; i < MAX_SOUND_BITES; i++) {
		if (soundBites[i].pSound == NULL) {
			soundBites[i].pSound = pSound;
			soundBites[i].location = 0;
			break;
		}
	}

	pthread_mutex_unlock(&audioMutex);
}

void AudioMixer_cleanup(void)
{
	printf("Stopping audio...\n");

	// Stop the PCM generation thread
	stopping = true;
	pthread_join(playbackThreadId, NULL);

	// Shutdown the PCM output, allowing any pending sound to play out (drain)
	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	// Free playback buffer
	// (note that any wave files read into wavedata_t records must be freed
	//  in addition to this by calling AudioMixer_freeWaveFileData() on that struct.)
	free(playbackBuffer);
	playbackBuffer = NULL;

	printf("Done stopping audio...\n");
	fflush(stdout);
}


int AudioMixer_getVolume()
{
	// Return the cached volume; good enough unless someone is changing
	// the volume through other means and the cached value is out of date.
	return volume;
}

// Function copied from:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
// Written by user "trenki".
void AudioMixer_setVolume(int newVolume)
{
	// Ensure volume is reasonable; If so, cache it for later getVolume() calls.
	if (newVolume < 0 || newVolume > AUDIOMIXER_MAX_VOLUME) {
		printf("ERROR: Volume must be between 0 and 100.\n");
		return;
	}
	volume = newVolume;

    long min, max;
    snd_mixer_t *volHandle;
    snd_mixer_selem_id_t *sid;
    const char *card = "default";
    const char *selem_name = "PCM";

    snd_mixer_open(&volHandle, 0);
    snd_mixer_attach(volHandle, card);
    snd_mixer_selem_register(volHandle, NULL, NULL);
    snd_mixer_load(volHandle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(volHandle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(volHandle);
}

int play=0;
// Fill the `buff` array with new PCM values to output.
//    `buff`: buffer to fill with new PCM data from sound bites.
//    `size`: the number of values to store into playbackBuffer
static void fillPlaybackBuffer(short *buff, int size)
{
	/*
	 * REVISIT: Implement this
	 * 1. Wipe the playbackBuffer to all 0's to clear any previous PCM data.
	 *    Hint: use memset()
	 * 2. Since this is called from a background thread, and soundBites[] array
	 *    may be used by any other thread, must synchronize this.
	 * 3. Loop through each slot in soundBites[], which are sounds that are either
	 *    waiting to be played, or partially already played:
	 *    - If the sound bite slot is unused, do nothing for this slot.
	 *    - Otherwise "add" this sound bite's data to the play-back buffer
	 *      (other sound bites needing to be played back will also add to the same data).
	 *      * Record that this portion of the sound bite has been played back by incrementing
	 *        the location inside the data where play-back currently is.
	 *      * If you have now played back the entire sample, free the slot in the
	 *        soundBites[] array.
	 *
	 * Notes on "adding" PCM samples:
	 * - PCM is stored as signed shorts (between SHRT_MIN and SHRT_MAX).
	 * - When adding values, ensure there is not an overflow. Any values which would
	 *   greater than SHRT_MAX should be clipped to SHRT_MAX; likewise for underflow.
	 * - Don't overflow any arrays!
	 * - Efficiency matters here! The compiler may do quite a bit for you, but it doesn't
	 *   hurt to keep it in mind. Here are some tips for efficiency and readability:
	 *   * If, for each pass of the loop which "adds" you need to change a value inside
	 *     a struct inside an array, it may be faster to first load the value into a local
	 *      variable, increment this variable as needed throughout the loop, and then write it
	 *     back into the struct inside the array after.
	 */

   memset(buff, 0, size * sizeof(*buff));
	
    pthread_mutex_lock(&audioMutex);
	
    for (int i = 0; i < MAX_SOUND_BITES; i++)
    {
        if (soundBites[i].pSound && soundBites[i].location < soundBites[i].pSound->numSamples)
        {
            wavedata_t *sound = soundBites[i].pSound;
            int remaining = sound->numSamples - soundBites[i].location;
            int samples = remaining > size ? size : remaining;
            short *src = sound->pData + soundBites[i].location;
            short *dst = buff;

            for (int j = 0; j < samples; j++)
            {
                int sum = (int)(*dst) + (*src++);
                sum = sum < SHRT_MIN ? SHRT_MIN : (sum > SHRT_MAX ? SHRT_MAX : sum);
                *dst++ = (short)sum;
            }

            soundBites[i].location += samples;

            if (soundBites[i].location >= sound->numSamples)
            {
                soundBites[i].pSound = NULL;
                soundBites[i].location = 0;
            }
        }
    }

    pthread_mutex_unlock(&audioMutex);
}



void* playbackThread(void* arg)
{
    while (!stopping)
    {
        if (isPlaying)
        {
            // Generate next block of audio
            fillPlaybackBuffer(playbackBuffer, playbackBufferSize);

            // Output the audio
            snd_pcm_sframes_t frames = snd_pcm_writei(handle,
                                                      playbackBuffer, playbackBufferSize);
            // Check for (and handle) possible error conditions on output
            if (frames < 0)
            {
                fprintf(stderr, "AudioMixer: writei() returned %li\n", frames);
                frames = snd_pcm_recover(handle, frames, 1);
            }
            if (frames < 0)
            {
                fprintf(stderr, "ERROR: Failed writing audio with snd_pcm_writei(): %li\n",
                        frames);
                exit(EXIT_FAILURE);
            }
            if (frames > 0 && frames < playbackBufferSize)
            {
                printf("Short write (expected %li, wrote %li)\n",
                       playbackBufferSize, frames);
            }

            // Check if the current sound has finished playing
            if (soundBites[0].pSound == NULL)
            {
                if (isLooping)
                {
                    // Loop the sound
                    SoundHandler_playDefaultSound(Responses_getAlarmMode());
                }
                else
                {
                    isPlaying = 0; // Reset the isPlaying flag
                }
            }
        }
        else
        {
            // Sleep for a while before checking again
            usleep(100000); // Adjust the sleep duration as needed
        }
    }

    return NULL;
}


void SoundHandler_playDefaultSound(enum ALARM_MODE alarm)
{
    printf("Playing alarm %d\r\n", alarm);

    // Set isPlaying and isLooping flags
    isPlaying = 1;
    isLooping = 1;

    switch (alarm)
    {
        case DEFAULT1:
            AudioMixer_queueSound(&def_alarmData1);
            break;
        case DEFAULT2:
            AudioMixer_queueSound(&def_alarmData2);
            break;
        case DEFAULT3:
            AudioMixer_queueSound(&def_alarmData3);
            break;
        case CUSTOM1:
            AudioMixer_queueSound(&cst_alarmData1);
            break;
        case CUSTOM2:
            AudioMixer_queueSound(&cst_alarmData2);
            break;
        case RICKROLL:
            AudioMixer_queueSound(&rickrollData);
            break;
        case PUNJABI:
            AudioMixer_queueSound(&punjabiData);
            break;
		case TEMP:
    	AudioMixer_readWaveFileIntoMemory("temp_resampled.wav", &soundData);
    	AudioMixer_queueSound(&soundData);
		int soundDurationMs = (soundData.numSamples * 1000) / 44100;
    	usleep(soundDurationMs * 1000);
		isPlaying = 0;
    	isLooping = 0;
   		AudioMixer_freeWaveFileData(&soundData);
        case STOP:
        default:
            soundBites[0].pSound = NULL;
            soundBites[0].location = 0;
            isPlaying = 0;
            isLooping = 0;
            break;
    }
}

