#ifndef _VAD_H
#define _VAD_H
#include <stdio.h>

/* TODO: add the needed states DONE*/
typedef enum
{
   ST_UNDEF = 0,
   ST_SILENCE = 1,
   ST_VOICE = 2,
   ST_MAYBE_VOICE = 3,
   ST_MAYBE_SILENCE = 4,
   ST_INIT = 5
} VAD_STATE;

/* Return a string label associated to each state */
const char *state2str(VAD_STATE st);

/* TODO: add the variables needed to control the VAD
   (counts, thresholds, etc.) DONE*/

typedef struct
{
   VAD_STATE state;
   float sampling_rate;
   unsigned int frame_length;
   float last_feature; /* for debuggin purposes */
   // added properties below (taken from pdf section 1.2)

   //***SILENCE/VOICE POWER THRESHOLD***
   float k0; // <-- if threshold is surpassed above -> voice | if its surpassed below -> silence ||| la dic k0 per consistencia amb el pdf

   //***MINIMUM SILENCE/VOICE DURATIONS FOR STATE CHANGE***
   unsigned int silence_standby; // <-- minimum amount of frames to wait b4 considering it as silence
   unsigned int voice_standby;   // <-- minimum amount of frames to wait b4 considering it as voice

   //***NUMBER OF FRAMES TO USE FOR INITIAL NOISE REFERENCE CALCULATION (N_init)***
   unsigned int initial_standby;

   //***NUMBER OF FRAMES IN WHICH THE VAD HAS BEEN IN A GIVEN STATE***
   unsigned int frames_so_far;

   //***ADVANCED PROPERTIES (NOISE REFENCE CALCULATION METHODS, THRESHOLD CALCULATIONS, ETC)***

   //***METHOD OF NOISE REFERENCE CALCULATION***
   //char noise_reference_calculation;
} VAD_DATA;

/* Call this function before using VAD:
   It should return allocated and initialized values of vad_data

   sampling_rate: ... the sampling rate */
VAD_DATA *vad_open(float sampling_rate);

/* vad works frame by frame.
   This function returns the frame size so that the program knows how
   many samples have to be provided */
unsigned int vad_frame_size(VAD_DATA *);

/* Main function. For each 'time', compute the new state
   It returns:
    ST_UNDEF   (0) : undefined; it needs more frames to take decission
    ST_SILENCE (1) : silence
    ST_VOICE   (2) : voice

    x: input frame
       It is assumed the length is frame_length */
VAD_STATE vad(VAD_DATA *vad_data, float *x/*, float alpha1 /*lab*/);

/* Free memory
   Returns the state of the last (undecided) states. */
VAD_STATE vad_close(VAD_DATA *vad_data);

/* Print actual state of vad, for debug purposes */
void vad_show_state(const VAD_DATA *, FILE *);

#endif
